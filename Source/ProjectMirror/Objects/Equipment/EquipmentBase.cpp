// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "EquipmentBase.h"

#include "ProjectMirror/Characters/Components/EquipmentComponent.h"
#include "ProjectMirror/SaveSystem/SaveGameData.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"
#include "ProjectMirror/Objects/InteractablePickupObject.h"
#include "ProjectMirror/StatusEffects/EffectComponentBase.h"


AEquipmentBase::AEquipmentBase()
{
	PrimaryActorTick.bCanEverTick = false;
}


void AEquipmentBase::BeginDrop(const FEquipmentBundle& DataBundle)
{
	if (!DataBundle.EventID.IsNone())
	{
		if (USubsystemObjectEvents* ObjectEventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
		{
			ObjectEventSubsystem->SetObjectActivated(DataBundle.EventID, false);
		}
	}

	if (!DataBundle.Definition)
	{
		Destroy();
		return;
	}
	UClass* PickupClass = (DataBundle.Definition->GetWorldPickUpActor().LoadSynchronous());
	if (!IsValid(PickupClass))
	{
		Destroy();
		return;
	}
	AInteractablePickupObject* PickupObject = GetWorld()->SpawnActorDeferred<AInteractablePickupObject>(PickupClass, GetActorTransform());
	if (IsValid(PickupObject))
	{
		PickupObject->SetFalling();
		PickupObject->SetEventID(DataBundle.EventID);
		PickupObject->FinishSpawning(GetActorTransform());
	}

	Destroy();
}

void AEquipmentBase::OnSlotChanged(const EEquipmentSlot NewSlot)
{
}

TArray<FSavedEffectData> AEquipmentBase::GetEquipmentEffectsForSave() const
{
	TArray<FSavedEffectData> Out;
	TArray<UEffectComponentBase*> Effects;
	GetComponents<UEffectComponentBase>(Effects);

	for (const UEffectComponentBase* Effect : Effects)
	{
		FSavedEffectData Entry;
		Entry.EffectClass = Effect->GetClass();
		Entry.bIsActive = Effect->GetEffectActive();
		Out.Add(Entry);
	}
	return Out;
}

void AEquipmentBase::ApplyEffectsFromSave(const TArray<FSavedEffectData>& SavedEffects)
{
	for (const FSavedEffectData& Entry : SavedEffects)
	{
		if (!Entry.EffectClass)
		{
			continue;
		}

		UEffectComponentBase* Existing = FindEffectOfClass(Entry.EffectClass);
		if (Existing)
		{
			Existing->SetEffectActive(Entry.bIsActive);
		}
		else
		{
			UEffectComponentBase* NewEffect = NewObject<UEffectComponentBase>(this, Entry.EffectClass);
			NewEffect->RegisterComponent();
			NewEffect->SetEffectActive(Entry.bIsActive);
		}
	}
}

UEffectComponentBase* AEquipmentBase::FindEffectOfClass(TSubclassOf<UEffectComponentBase> EffectClass) const
{
	if (!EffectClass)
	{
		return nullptr;
	}

	TArray<UEffectComponentBase*> Effects;
	GetComponents<UEffectComponentBase>(Effects);
	for (UEffectComponentBase* Effect : Effects)
	{
		if (Effect->GetClass() == EffectClass)
		{
			return Effect;
		}
	}
	return nullptr;
}

void AEquipmentBase::ApplyInitialEffectStates(const TMap<TSubclassOf<UEffectComponentBase>, bool>& States)
{
	for (const TPair<TSubclassOf<UEffectComponentBase>, bool>& EffectStatePair : States)
	{
		if (UEffectComponentBase* FoundEffect = FindEffectOfClass(EffectStatePair.Key))
		{
			FoundEffect->SetEffectActive(EffectStatePair.Value);
		}
	}
}

void AEquipmentBase::AttachToOwner(USkeletalMeshComponent* OwnerMesh, const FName& Socket)
{
	if (!IsValid(OwnerMesh))
	{
		return;
	}
	AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
}

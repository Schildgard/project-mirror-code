// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractableFireplace.h"

#include "NiagaraComponent.h"
#include "Components/PointLightComponent.h"
#include "Equipment/EquipmentBase.h"
#include "ProjectMirror/Characters/Components/EquipmentComponent.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"


AInteractableFireplace::AInteractableFireplace()
{
	InteractionCategory = EInteractionCategory::Ignite;
	bIsOneTimeInteraction = false;

	bIsIgnited = false;
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraComponent");
	NiagaraComponent->SetupAttachment(StaticMeshComponent);
	
	LightSource = CreateDefaultSubobject<UPointLightComponent>("LightSource");
	LightSource->SetupAttachment(NiagaraComponent);
}

void AInteractableFireplace::BeginPlay()
{
	Super::BeginPlay();

	const bool bDefaultIgnitionState = bIsIgnited; //Save Editor State before overriding

	bool bToggleFromDefault = false;
	if (!EventID.IsNone())
	{
		if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
		{
			bToggleFromDefault = EventSubsystem->IsObjectActivated(EventID);
		}
	}

	if (bDefaultIgnitionState != bToggleFromDefault)
	{
		Ignite();
	}
	else
	{
		Extinguish();
	}
}

void AInteractableFireplace::Ignite()
{
	bIsIgnited = true;
	if (IsValid(NiagaraComponent))
	{
		NiagaraComponent->SetActive(true);
	}
	if (IsValid(LightSource))
	{
		LightSource->SetVisibility(true);
	}
}

void AInteractableFireplace::Extinguish()
{
	bIsIgnited = false;
	if (IsValid(NiagaraComponent))
	{
		NiagaraComponent->SetActive(false);
	}
	if (IsValid(LightSource))
	{
		LightSource->SetVisibility(false);
	}
}

IIgnitable* AInteractableFireplace::ResolveIgnitableWeapon(UInteractionComponent* InteractionComponent) const
{
	if (!IsValid(InteractionComponent))
	{
		return nullptr;
	}

	AActor* InteractingActor = InteractionComponent->GetOwner();
	UEquipmentComponent* Equipment = InteractingActor ? InteractingActor->FindComponentByClass<UEquipmentComponent>() : nullptr;
	if (!Equipment)
	{
		return nullptr;
	}

	const bool bWantIgnitedWeapon = !bIsIgnited;

	const EEquipmentSlot IgnitableSlot = Equipment->ResolveIgnitableSlot(bWantIgnitedWeapon);
	const AEquipmentBase* IgnitableWeapon = Equipment->GetActiveWeaponActor(IgnitableSlot);
	if (IsValid(IgnitableWeapon))
	{
		IIgnitable* IgnitableInterface = Cast<IIgnitable>(IgnitableWeapon->FindComponentByInterface(UIgnitable::StaticClass()));
		if (IgnitableInterface)
		{
			return IgnitableInterface;
		}
	}


	return nullptr;
}

bool AInteractableFireplace::IsInterActionPossible_Implementation(UInteractionComponent* InteractionComponent)
{
	if (!Super::IsInterActionPossible_Implementation(InteractionComponent))
	{
		return false;
	}
	return ResolveIgnitableWeapon(InteractionComponent) != nullptr;
}

void AInteractableFireplace::OnInteract_Implementation(UInteractionComponent* InteractionComponent)
{
	IIgnitable* Weapon = ResolveIgnitableWeapon(InteractionComponent);
	if (!Weapon)
	{
		return;
	}
	if (bIsIgnited)
	{
		//Fireplace Lits Torch. Do not save. Or...should save?
		Weapon->Ignite();
	}
	else
	{
		Ignite();
		Super::OnInteract_Implementation(InteractionComponent);
	}
}

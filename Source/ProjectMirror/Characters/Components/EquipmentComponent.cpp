// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "EquipmentComponent.h"

#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "ProjectMirror/Data/DefinitionEquipment.h"
#include "ProjectMirror/Interfaces/Ignitable.h"
#include "ProjectMirror/Objects/InteractablePickupObject.h"
#include "ProjectMirror/Objects/Equipment/EquipmentBase.h"
#include "ProjectMirror/Objects/Equipment/EquipmentSkeletal.h"
#include "ProjectMirror/Objects/Equipment/EquipmentStatic.h"
#include "ProjectMirror/SaveSystem/SaveGameData.h"
#include "ProjectMirror/Settings/SettingsObjects.h"

static constexpr const TCHAR* MainHandSocketName = TEXT("HandGrip_R");
static constexpr const TCHAR* OffhandSocketName = TEXT("HandGrip_L");
static constexpr const TCHAR* BackupSocketName = TEXT("PelvisGrip");
static constexpr const TCHAR* BackupTwoHandedSocketName = TEXT("BackGrip");
static constexpr const TCHAR* BackupShieldSocketName = TEXT("BackGrip_Shield");
static constexpr const TCHAR* ShieldSocketName = TEXT("HandGrip_Shield");

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedOwnerMesh = GetOwnerMesh();
}

FEquipmentBundle UEquipmentComponent::ResolveSlotInDataBundle(EEquipmentSlot Slot)
{
	FEquipmentBundle EquipmentDataBundle;
	CurrentEquipmentMap.RemoveAndCopyValue(Slot, EquipmentDataBundle);

	return EquipmentDataBundle;
}

EEquipmentSlot UEquipmentComponent::EquipAndReturnResolvedSlot(UDefinitionEquipment* EquipmentDefinition, const FName& EventID, bool bHideEquipmentOnSpawn)
{
	if (!IsValid(CachedOwnerMesh) || !EquipmentDefinition || EquipmentDefinition->GetEquipmentSlot() == EEquipmentSlot::Undefined)
	{
		return EEquipmentSlot::Undefined;
	}

	EEquipmentSlot Slot = ResolveEquipmentSlot(EquipmentDefinition);
	if (CurrentEquipmentMap.Contains(Slot))
	{
		Unequip(Slot);
	}
	else if (Slot == EEquipmentSlot::TwoHanded)
	{
		Unequip(EEquipmentSlot::MainHand);
		Unequip(EEquipmentSlot::OffHand);
	}
	else if (CurrentEquipmentMap.Contains(EEquipmentSlot::TwoHanded) && (Slot == EEquipmentSlot::MainHand || Slot == EEquipmentSlot::OffHand))
	{
		Unequip(EEquipmentSlot::TwoHanded);
	}

	AEquipmentBase* EquipmentActor = SpawnEquipmentActor(EquipmentDefinition, Slot);
	if (!IsValid(EquipmentActor))
	{
		return EEquipmentSlot::Undefined;
	}


	FEquipmentBundle Bundle;
	Bundle.EventID = EventID;
	Bundle.Definition = EquipmentDefinition;
	Bundle.EquipmentActor = EquipmentActor;
	CurrentEquipmentMap.Add(Slot, Bundle);

	if (bHideEquipmentOnSpawn)
	{
		EquipmentActor->GetMesh()->SetVisibility(false);
	}

	UpdateDefenseStats();
	OnEquipmentChanged.Broadcast(Slot);
	bool bIsBackupSlot = Slot == EEquipmentSlot::BackupPrimary || Slot == EEquipmentSlot::BackupSecondary || Slot == EEquipmentSlot::BackupTwoHanded;
	if (EquipmentDefinition->GetIsWeapon() && !bIsBackupSlot)
	{
		OnWeaponChanged.Broadcast(EquipmentDefinition->GetWeaponType(), Slot == EEquipmentSlot::OffHand);
	}
	return Slot;
}

void UEquipmentComponent::Unequip(EEquipmentSlot EquipmentSlot)
{
	if (CurrentEquipmentMap.Contains(EquipmentSlot))
	{
		MarkEquipmentForShow(EquipmentSlot); //If Object was marked for Hide on Pick Up, it is removed from this list before the Hide notify fires.
		const FEquipmentBundle EquipmentDataBundle = ResolveSlotInDataBundle(EquipmentSlot);
		DropEquipment(EquipmentDataBundle);
		if (EquipmentDataBundle.Definition->GetIsWeapon())
		{
			OnWeaponChanged.Broadcast(EWeaponType::None, EquipmentSlot == EEquipmentSlot::OffHand);
		}
	}
	UpdateDefenseStats();
	OnEquipmentChanged.Broadcast(EquipmentSlot);
}

EEquipmentSlot UEquipmentComponent::ResolveEquipmentSlot(const UDefinitionEquipment* WeaponToEquip) const
{
	if (!WeaponToEquip)
	{
		return EEquipmentSlot::Undefined;
	}
	if (!WeaponToEquip->GetIsWeapon())
	{
		return WeaponToEquip->GetEquipmentSlot();
	}
	const bool bIsTwoHanded = WeaponToEquip->GetEquipmentSlot() == EEquipmentSlot::TwoHanded;
	const bool bIsShield = WeaponToEquip->GetWeaponType() == EWeaponType::Shield;

	const bool bTwoHanderEquipped = CurrentEquipmentMap.Contains(EEquipmentSlot::TwoHanded);
	const bool bRightHandFree = !bTwoHanderEquipped && !CurrentEquipmentMap.Contains(EEquipmentSlot::MainHand);
	const bool bOffHandFree = !bTwoHanderEquipped && !CurrentEquipmentMap.Contains(EEquipmentSlot::OffHand);

	if (bIsTwoHanded)
	{
		if (bRightHandFree && bOffHandFree)
		{
			return EEquipmentSlot::TwoHanded;
		}
	}
	else if (bIsShield)
	{
		if (bOffHandFree)
		{
			return EEquipmentSlot::OffHand;
		}
	}
	else
	{
		if (bRightHandFree) { return EEquipmentSlot::MainHand; }
		if (bOffHandFree) { return EEquipmentSlot::OffHand; }
	}
	const EEquipmentSlot BackupSlot = FindViableBackupSlot(WeaponToEquip);
	if (BackupSlot != EEquipmentSlot::Undefined)
	{
		return BackupSlot;
	}


	if (bIsTwoHanded) { return EEquipmentSlot::TwoHanded; }
	if (bIsShield) { return EEquipmentSlot::OffHand; }
	return WeaponToEquip->GetEquipmentSlot();
}

FName UEquipmentComponent::ResolveEquipmentAttachmentSocketName(const UDefinitionEquipment* EquipmentDefinition, EEquipmentSlot Slot) const
{
	if (!EquipmentDefinition->GetIsWeapon())
	{
		return EquipmentDefinition->AttachSocket;
	}

	switch (Slot)
	{
	case EEquipmentSlot::TwoHanded:
	case EEquipmentSlot::MainHand:
		{
			return MainHandSocketName;
		}
	case EEquipmentSlot::OffHand:
		{
			return EquipmentDefinition->GetWeaponType() == EWeaponType::Shield ? ShieldSocketName : OffhandSocketName;
		}
	case EEquipmentSlot::BackupPrimary:
		{
			return BackupSocketName;
		}
	case EEquipmentSlot::BackupSecondary:
		{
			if (EquipmentDefinition->GetWeaponType() == EWeaponType::Shield)
			{
				return BackupShieldSocketName;
			}
			return BackupTwoHandedSocketName;
		}
	case EEquipmentSlot::BackupTwoHanded:
		{
			return BackupTwoHandedSocketName;
		}

	default:
		{
			return MainHandSocketName;
		}
	}
}

UDefinitionEquipment* UEquipmentComponent::GetEquipmentDefinition(const EEquipmentSlot Slot) const
{
	if (const FEquipmentBundle* Found = CurrentEquipmentMap.Find(Slot))
	{
		return Found->Definition;
	}
	return nullptr;
}

USkeletalMeshComponent* UEquipmentComponent::GetOwnerMesh() const
{
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		return Character->GetMesh();
	}
	return nullptr;
}

AEquipmentBase* UEquipmentComponent::SpawnEquipmentActor(UDefinitionEquipment* EquipmentDefinition, EEquipmentSlot Slot) const
{
	UWorld* World = GetWorld();
	if (!World || !IsValid(CachedOwnerMesh))
	{
		return nullptr;
	}
	TSubclassOf<AEquipmentBase> EquipmentClass;
	if (EquipmentDefinition->GetIsFlexible())
	{
		EquipmentClass = EquipmentDefinition->SkeletalEquipmentClass;
	}
	else
	{
		EquipmentClass = EquipmentDefinition->StaticEquipmentClass;
	}
	if (!IsValid(EquipmentClass))
	{
		return nullptr;
	}

	AEquipmentBase* Equipment = World->SpawnActorDeferred<AEquipmentBase>(EquipmentClass, FTransform::Identity, GetOwner(), Cast<APawn>(GetOwner()));

	if (!IsValid(Equipment))
	{
		return nullptr;
	}

	const FName AttachSocketName = ResolveEquipmentAttachmentSocketName(EquipmentDefinition, Slot);

	Equipment->SetSourceDefinition(EquipmentDefinition);
	Equipment->FinishSpawning(FTransform::Identity);
	Equipment->AttachToOwner(CachedOwnerMesh, AttachSocketName);

	return Equipment;
}

void UEquipmentComponent::HandleSwitchWeaponInput(const FInputActionValue& Input)
{
	const float InputValue = Input.Get<float>();

	if (InputValue > 0)
	{
		SwapWeapons(EEquipmentSlot::MainHand);
	}
	else if (InputValue < 0)
	{
		SwapWeapons(EEquipmentSlot::OffHand);
	}
}

void UEquipmentComponent::SwapWeaponSlots(EEquipmentSlot SlotA, EEquipmentSlot SlotB)
{
	if (SlotA == SlotB || !IsValid(CachedOwnerMesh))
	{
		return;
	}

	FEquipmentBundle EquipmentDataBundleA = ResolveSlotInDataBundle(SlotA);
	FEquipmentBundle EquipmentDataBundleB = ResolveSlotInDataBundle(SlotB);

	PlaceEquipmentInSlot(SlotA, EquipmentDataBundleB);
	PlaceEquipmentInSlot(SlotB, EquipmentDataBundleA);
}

void UEquipmentComponent::SwapWeapons(EEquipmentSlot ActiveSlot)
{
	const bool bTwoHandedInvolved = CurrentEquipmentMap.Contains(EEquipmentSlot::TwoHanded) || CurrentEquipmentMap.Contains(EEquipmentSlot::BackupTwoHanded);
	if (bTwoHandedInvolved)
	{
		SwapWeaponSlots(EEquipmentSlot::MainHand, EEquipmentSlot::BackupPrimary);
		SwapWeaponSlots(EEquipmentSlot::OffHand, EEquipmentSlot::BackupSecondary);
		SwapWeaponSlots(EEquipmentSlot::TwoHanded, EEquipmentSlot::BackupTwoHanded);
	}
	else
	{
		const EEquipmentSlot DrawSlot = DrawFromBackupSlot(ActiveSlot);
		FEquipmentBundle HandBundle = ResolveSlotInDataBundle(ActiveSlot);
		FEquipmentBundle DrawBundle = (DrawSlot != EEquipmentSlot::Undefined) ? ResolveSlotInDataBundle(DrawSlot) : FEquipmentBundle();

		PlaceEquipmentInSlot(ActiveSlot, DrawBundle);
		if (HandBundle.Definition)
		{
			const EEquipmentSlot StowSlot = FindViableBackupSlot(HandBundle.Definition);
			if (StowSlot != EEquipmentSlot::Undefined)
			{
				PlaceEquipmentInSlot(StowSlot, HandBundle);
			}
		}
	}

	BroadcastHandState();
}

void UEquipmentComponent::PlaceEquipmentInSlot(EEquipmentSlot TargetSlot, const FEquipmentBundle& EquipmentBundle)
{
	if (!IsValid(EquipmentBundle.Definition) || !IsValid(CachedOwnerMesh))
	{
		return;
	}
	CurrentEquipmentMap.Add(TargetSlot, EquipmentBundle);
	if (IsValid(EquipmentBundle.EquipmentActor))
	{
		EquipmentBundle.EquipmentActor->AttachToOwner(CachedOwnerMesh, ResolveEquipmentAttachmentSocketName(EquipmentBundle.Definition, TargetSlot));
		EquipmentBundle.EquipmentActor->OnSlotChanged(TargetSlot);
	}
}

EEquipmentSlot UEquipmentComponent::DrawFromBackupSlot(EEquipmentSlot Hand) const
{
	const EEquipmentSlot PairedBackupSlot = (Hand == EEquipmentSlot::MainHand) ? EEquipmentSlot::BackupPrimary : EEquipmentSlot::BackupSecondary;
	const EEquipmentSlot OtherBackupSlot = (Hand == EEquipmentSlot::MainHand) ? EEquipmentSlot::BackupSecondary : EEquipmentSlot::BackupPrimary;

	const bool bIsRightHand = (Hand == EEquipmentSlot::MainHand);

	if (CurrentEquipmentMap.Contains(PairedBackupSlot) && !(bIsRightHand && IsShieldInSlot(PairedBackupSlot)))
	{
		return PairedBackupSlot;
	}
	if (CurrentEquipmentMap.Contains(OtherBackupSlot) && !(bIsRightHand && IsShieldInSlot(OtherBackupSlot)))
	{
		return OtherBackupSlot;
	}
	return EEquipmentSlot::Undefined;
}

bool UEquipmentComponent::IsShieldInSlot(EEquipmentSlot Slot) const
{
	const UDefinitionEquipment* Def = GetEquipmentDefinition(Slot);
	return Def && Def->GetWeaponType() == EWeaponType::Shield;
}


EEquipmentSlot UEquipmentComponent::FindViableBackupSlot(const UDefinitionEquipment* Weapon) const
{
	if (!Weapon)
	{
		return EEquipmentSlot::Undefined;
	}

	const bool bIsWeaponTwoHanded = Weapon->GetEquipmentSlot() == EEquipmentSlot::TwoHanded;
	const bool bIsWeaponShield = Weapon->GetWeaponType() == EWeaponType::Shield;

	const bool bPrimaryFree = !CurrentEquipmentMap.Contains(EEquipmentSlot::BackupPrimary);
	const bool bSecondaryFree = !CurrentEquipmentMap.Contains(EEquipmentSlot::BackupSecondary);
	const bool bTwoHandedFree = !CurrentEquipmentMap.Contains(EEquipmentSlot::BackupTwoHanded);

	if (bIsWeaponTwoHanded)
	{
		return (bPrimaryFree && bSecondaryFree && bTwoHandedFree) ? EEquipmentSlot::BackupTwoHanded : EEquipmentSlot::Undefined;
	}

	if (!bTwoHandedFree)
	{
		return EEquipmentSlot::Undefined;
	}

	if (bIsWeaponShield)
	{
		return bSecondaryFree ? EEquipmentSlot::BackupSecondary : EEquipmentSlot::Undefined;
	}
	if (bPrimaryFree)
	{
		return EEquipmentSlot::BackupPrimary;
	}
	return bSecondaryFree ? EEquipmentSlot::BackupSecondary : EEquipmentSlot::Undefined;
}

void UEquipmentComponent::BroadcastHandState() const
{
	const UDefinitionEquipment* MainWeaponDef = GetEquipmentDefinition(EEquipmentSlot::TwoHanded);
	if (!MainWeaponDef)
	{
		MainWeaponDef = GetEquipmentDefinition(EEquipmentSlot::MainHand);
	}
	const EWeaponType MainWeaponType = MainWeaponDef ? MainWeaponDef->GetWeaponType() : EWeaponType::None;

	const UDefinitionEquipment* OffDef = GetEquipmentDefinition(EEquipmentSlot::OffHand);
	const EWeaponType OffWeaponType = OffDef ? OffDef->GetWeaponType() : EWeaponType::None;

	OnWeaponChanged.Broadcast(MainWeaponType, false);
	OnWeaponChanged.Broadcast(OffWeaponType, true);
}

void UEquipmentComponent::DropEquipment(const FEquipmentBundle& DataBundle)
{
	AEquipmentBase* DiscardedEquipment = DataBundle.EquipmentActor;
	if (!IsValid(DiscardedEquipment))
	{
		return;
	}
	DiscardedEquipment->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	DiscardedEquipment->BeginDrop(DataBundle);
}

void UEquipmentComponent::ClearAllEquipment()
{
	for (auto& Pair : CurrentEquipmentMap)
	{
		if ((Pair.Value.EquipmentActor))
		{
			Pair.Value.EquipmentActor->Destroy();
		}
	}
	CurrentEquipmentMap.Empty();
	UpdateDefenseStats();
	BroadcastHandState();
}

TMap<EEquipmentSlot, FSavedEquipmentData> UEquipmentComponent::GetEquipmentForSave() const
{
	TMap<EEquipmentSlot, FSavedEquipmentData> SaveData;

	for (const TPair<EEquipmentSlot, FEquipmentBundle>& Pair : CurrentEquipmentMap)
	{
		const FEquipmentBundle& EquipmentBundle = Pair.Value;

		FSavedEquipmentData SavedEquipmentData;
		SavedEquipmentData.EventID = EquipmentBundle.EventID;
		SavedEquipmentData.Definition = EquipmentBundle.Definition;
		if (IsValid(EquipmentBundle.EquipmentActor))
		{
			SavedEquipmentData.ActiveEffects = EquipmentBundle.EquipmentActor->GetEquipmentEffectsForSave();
		}

		SaveData.Add(Pair.Key, SavedEquipmentData);
	}
	return SaveData;
}

void UEquipmentComponent::MarkEquipmentForHide(EEquipmentSlot Slot)
{
	if (FEquipmentBundle* EquipmentDataPtr = CurrentEquipmentMap.Find(Slot))
	{
		TemporaryHiddenEquipmentMap.Add(Slot, *EquipmentDataPtr);
	}
}

void UEquipmentComponent::MarkEquipmentForShow(EEquipmentSlot Slot)
{
	if (TemporaryHiddenEquipmentMap.Find(Slot))
	{
		TemporaryHiddenEquipmentMap.Remove(Slot);
	}
}

EEquipmentSlot UEquipmentComponent::ResolveEquipmentSlotFromBackup(const EEquipmentSlot BackupSlot)
{
	switch (BackupSlot)
	{
	case EEquipmentSlot::BackupPrimary:
		return EEquipmentSlot::MainHand;
	case EEquipmentSlot::BackupSecondary:
		return EEquipmentSlot::OffHand;
	case EEquipmentSlot::BackupTwoHanded:
		return (EEquipmentSlot::TwoHanded);
	default:
		return EEquipmentSlot::Undefined;
	}
}

void UEquipmentComponent::UpdateDefenseStats()
{
	FDefenseStats NewDefenseStats;

	for (const TPair<EEquipmentSlot, FEquipmentBundle>& Pair : CurrentEquipmentMap)
	{
		if (!IsValid(Pair.Value.EquipmentActor) || !IsValid(Pair.Value.Definition) || Pair.Value.Definition->GetIsWeapon())
		{
			continue;
		}
		const FDefenseStats EquipmentDefenseStats = Pair.Value.EquipmentActor->GetDefenseStats();

		NewDefenseStats.DefensePhysical += EquipmentDefenseStats.DefensePhysical;
		NewDefenseStats.DefenseDarkness += EquipmentDefenseStats.DefenseDarkness;
		NewDefenseStats.DefenseFire += EquipmentDefenseStats.DefenseFire;
		NewDefenseStats.DefenseLight += EquipmentDefenseStats.DefenseLight;
		NewDefenseStats.DefenseMagic += EquipmentDefenseStats.DefenseMagic;
		NewDefenseStats.Poise += EquipmentDefenseStats.Poise;
	}

	TotalDefenseStats = NewDefenseStats;
}

FName UEquipmentComponent::ResolveGrabHandSocket(EEquipmentSlot Slot) const
{
	switch (Slot)
	{
	case EEquipmentSlot::OffHand:
	case EEquipmentSlot::BackupSecondary:
		return OffhandSocketName;
	default:
		return MainHandSocketName;
	}
}

void UEquipmentComponent::ShowAllHiddenEquipment()
{
	for (auto& Pair : TemporaryHiddenEquipmentMap)
	{
		if (!IsValid(Pair.Value.EquipmentActor))
		{
			continue;
		}
		UMeshComponent* MeshComponent = Pair.Value.EquipmentActor->GetMesh();
		if (IsValid(MeshComponent))
		{
			MeshComponent->SetVisibility(true);
		}
	}
	TemporaryHiddenEquipmentMap.Empty();
}

void UEquipmentComponent::MarkPickUpMeshForDestroy(AInteractablePickupObject* PickupEquipment, const EEquipmentSlot Slot)
{
	if (!IsValid(PickupEquipment))
	{
		return;
	}
	CachedEquipmentPickUpMesh = PickupEquipment;
	CachedPickUpSlot = Slot;
}

void UEquipmentComponent::FinalizePickup()
{
	if (!IsValid(CachedEquipmentPickUpMesh)) { return; }

	const FEquipmentBundle* Bundle = CurrentEquipmentMap.Find(CachedPickUpSlot);
	UMeshComponent* WeaponMesh = (Bundle && IsValid(Bundle->EquipmentActor)) ? Bundle->EquipmentActor->GetMesh() : nullptr;

	const bool bIsBackupSlot =
		CachedPickUpSlot == EEquipmentSlot::BackupPrimary ||
		CachedPickUpSlot == EEquipmentSlot::BackupSecondary ||
		CachedPickUpSlot == EEquipmentSlot::BackupTwoHanded;

	if (bIsBackupSlot && Bundle && IsValid(CachedOwnerMesh))
	{
		const FName BackupSocket = ResolveEquipmentAttachmentSocketName(Bundle->Definition, CachedPickUpSlot);
		const float Duration = GetDefault<USettingsObjects>()->GetStowBlendDuration();
		CachedEquipmentPickUpMesh->StartStowBlend(CachedOwnerMesh, BackupSocket, WeaponMesh, Duration);
	}
	else
	{
		if (WeaponMesh) { WeaponMesh->SetVisibility(true); }
		CachedEquipmentPickUpMesh->Destroy();
	}
	CachedEquipmentPickUpMesh = nullptr;
	CachedPickUpSlot = EEquipmentSlot::Undefined;
}

void UEquipmentComponent::CommitPickupVisuals()
{
	ShowAllHiddenEquipment();
	FinalizePickup();
}

void UEquipmentComponent::SetEquipmentFromSave(const TMap<EEquipmentSlot, FSavedEquipmentData>& SavedEquipment)
{
	ClearAllEquipment();

	for (const auto& Pair : SavedEquipment)
	{
		const EEquipmentSlot Slot = Pair.Key;
		const FSavedEquipmentData& Entry = Pair.Value;
		if (!IsValid(Entry.Definition) || Slot == EEquipmentSlot::Undefined) { continue; }

		AEquipmentBase* Actor = SpawnEquipmentActor(Entry.Definition, Slot);
		if (!IsValid(Actor)) { continue; }
		Actor->ApplyEffectsFromSave(Entry.ActiveEffects);

		FEquipmentBundle Bundle;
		Bundle.EventID = Entry.EventID;
		Bundle.Definition = Entry.Definition;
		Bundle.EquipmentActor = Actor;
		CurrentEquipmentMap.Add(Slot, Bundle);
		OnEquipmentChanged.Broadcast(Slot);
	}
	UpdateDefenseStats();
	BroadcastHandState();
}

void UEquipmentComponent::HideActiveWeaponsForInteraction(const EInteractionCategory InteractionCategory, const EEquipmentSlot TargetEquipmentSlot)
{
	if (InteractionCategory == EInteractionCategory::Take)
	{
		EEquipmentSlot SlotToHide = ResolveEquipmentSlotFromBackup(TargetEquipmentSlot);
		if (SlotToHide == EEquipmentSlot::Undefined)
		{
			SlotToHide = TargetEquipmentSlot;
		}
		const bool bIsHandSlot = SlotToHide == EEquipmentSlot::MainHand || SlotToHide == EEquipmentSlot::OffHand || SlotToHide ==
			EEquipmentSlot::TwoHanded;
		if (SlotToHide == EEquipmentSlot::TwoHanded || !bIsHandSlot)
		{
			MarkEquipmentForHide(EEquipmentSlot::MainHand);
			MarkEquipmentForHide(EEquipmentSlot::OffHand);
			MarkEquipmentForHide(EEquipmentSlot::TwoHanded);
		}
		else
		{
			MarkEquipmentForHide(SlotToHide);
		}
	}
	else if (InteractionCategory == EInteractionCategory::Open)
	{
		MarkEquipmentForHide(EEquipmentSlot::MainHand);
		MarkEquipmentForHide(EEquipmentSlot::OffHand);
		MarkEquipmentForHide(EEquipmentSlot::TwoHanded);
	}
}

void UEquipmentComponent::DiscardMainWeapon()
{
	Unequip(EEquipmentSlot::MainHand);
}

void UEquipmentComponent::DiscardOffWeapon()
{
	Unequip(EEquipmentSlot::OffHand);
}

AEquipmentBase* UEquipmentComponent::GetActiveWeaponActor(EEquipmentSlot Hand) const
{
	if (const FEquipmentBundle* Bundle = CurrentEquipmentMap.Find(Hand))return Bundle->EquipmentActor;
	return nullptr;
}

EEquipmentSlot UEquipmentComponent::ResolveIgnitableSlot(const bool bWantIgnitedWeapon) const
{
	for (const EEquipmentSlot Hand : {EEquipmentSlot::MainHand, EEquipmentSlot::OffHand, EEquipmentSlot::TwoHanded})
	{
		AEquipmentBase* Weapon = GetActiveWeaponActor(Hand);
		if (!IsValid(Weapon))
		{
			continue;
		}
		IIgnitable* Ignitable = Cast<IIgnitable>(Weapon->FindComponentByInterface(UIgnitable::StaticClass()));
		if (Ignitable && Ignitable->IsIgnited() == bWantIgnitedWeapon)
		{
			return Hand;
		}
	}
	return EEquipmentSlot::Undefined;
}

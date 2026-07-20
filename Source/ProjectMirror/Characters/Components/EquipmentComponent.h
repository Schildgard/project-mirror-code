// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectMirror/Utility/CombatData.h"
#include "ProjectMirror/SaveSystem/SaveGameData.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "EquipmentComponent.generated.h"

class AInteractablePickupObject;
class AEquipmentBase;
struct FInputActionValue;
class UDefinitionEquipment;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, EEquipmentSlot, Slot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponChanged, EWeaponType, WeaponType, bool, bIsOffHand);

struct FEquipmentBlend
{
	TWeakObjectPtr<USceneComponent> Target;
	FTransform StartRelative = FTransform::Identity;
	float StartTime = 0.f;
	float Duration = 0.f;
	FTimerHandle TimerHandle;
};

USTRUCT()
struct FEquipmentBundle
{
	GENERATED_BODY()

	UPROPERTY()
	FName EventID = NAME_None;
	UPROPERTY()
	TObjectPtr<UDefinitionEquipment> Definition = nullptr;
	UPROPERTY()
	TObjectPtr<AEquipmentBase> EquipmentActor = nullptr;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<EEquipmentSlot, FEquipmentBundle> CurrentEquipmentMap;

	UPROPERTY()
	TMap<EEquipmentSlot, FEquipmentBundle> TemporaryHiddenEquipmentMap;

	UPROPERTY()
	AInteractablePickupObject* CachedEquipmentPickUpMesh;
	UPROPERTY()
	EEquipmentSlot CachedPickUpSlot = EEquipmentSlot::Undefined;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedOwnerMesh;

	FEquipmentBundle ResolveSlotInDataBundle(EEquipmentSlot Slot);

	UPROPERTY()
	FDefenseStats TotalDefenseStats;

public:
	EEquipmentSlot EquipAndReturnResolvedSlot(UDefinitionEquipment* EquipmentDefinition, const FName& EventID = NAME_None, bool bHideEquipmentOnSpawn = false);
	void Unequip(EEquipmentSlot EquipmentSlotDefinition);
	EEquipmentSlot ResolveEquipmentSlot(const UDefinitionEquipment* WeaponToEquip) const;
	FName ResolveEquipmentAttachmentSocketName(const UDefinitionEquipment* EquipmentDefinition, EEquipmentSlot Slot) const;

	UDefinitionEquipment* GetEquipmentDefinition(const EEquipmentSlot Slot) const;

	UPROPERTY()
	FOnEquipmentChanged OnEquipmentChanged;
	UPROPERTY()
	FOnWeaponChanged OnWeaponChanged;

	USkeletalMeshComponent* GetOwnerMesh() const;

	AEquipmentBase* SpawnEquipmentActor(UDefinitionEquipment* EquipmentDefinition, EEquipmentSlot Slot)const;

	TMap<EEquipmentSlot, FEquipmentBundle> GetCurrentEquipmentMap() const { return CurrentEquipmentMap; }
	TMap<EEquipmentSlot, FEquipmentBundle> GetHiddenEquipmentMap() const { return TemporaryHiddenEquipmentMap; }

	//Weapon Swap
	UFUNCTION()
	void HandleSwitchWeaponInput(const FInputActionValue& Input);
	void SwapWeaponSlots(EEquipmentSlot SlotA, EEquipmentSlot SlotB);
	void SwapWeapons(EEquipmentSlot ActiveSlot);
	void PlaceEquipmentInSlot(EEquipmentSlot TargetSlot, const FEquipmentBundle& EquipmentBundle);
	EEquipmentSlot DrawFromBackupSlot(EEquipmentSlot Hand) const;
	bool IsShieldInSlot(EEquipmentSlot Slot) const;

	UFUNCTION()
	void BroadcastHandState() const;
	void DropEquipment(const FEquipmentBundle& DataBundle);
	void ClearAllEquipment();

	void MarkEquipmentForHide(EEquipmentSlot Slot);
	void MarkEquipmentForShow(EEquipmentSlot Slot);
	void ShowAllHiddenEquipment();
	void MarkPickUpMeshForDestroy(AInteractablePickupObject* PickupEquipment, EEquipmentSlot Slot);
	void FinalizePickup();
	void CommitPickupVisuals();
	void SetEquipmentFromSave(const TMap<EEquipmentSlot, FSavedEquipmentData>& SavedEquipment);
	void HideActiveWeaponsForInteraction(const EInteractionCategory InteractionCategory, const EEquipmentSlot TargetEquipmentSlot);

	UFUNCTION()
	void DiscardMainWeapon();
	UFUNCTION()
	void DiscardOffWeapon();

	//Resolve Data
	AEquipmentBase* GetActiveWeaponActor(EEquipmentSlot Hand = EEquipmentSlot::MainHand) const;
	EEquipmentSlot ResolveIgnitableSlot(const bool bWantIgnitedWeapon) const;
	FName ResolveGrabHandSocket(EEquipmentSlot Slot) const;
	EEquipmentSlot FindViableBackupSlot(const UDefinitionEquipment* Weapon) const;
	TMap<EEquipmentSlot, FSavedEquipmentData> GetEquipmentForSave() const;
	EEquipmentSlot ResolveEquipmentSlotFromBackup(const EEquipmentSlot BackupSlot);
	
	void UpdateDefenseStats();
	FDefenseStats GetTotalDefenseStats()const {return TotalDefenseStats;};
};

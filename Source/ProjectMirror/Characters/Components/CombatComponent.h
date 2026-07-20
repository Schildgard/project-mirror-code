// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectMirror/Utility/CombatData.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "ProjectMirror/Utility/HitFeedbackData.h"
#include "CombatComponent.generated.h"


enum class EEquipmentSlot : uint8;
class AWeaponStaticBase;
class UAnimInstanceSimple;
class UAnimMontage;
class UEquipmentComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

	int32 AttackCount = 0;
	bool bIsComboWindowOpen = false;

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> CachedEquipmentComponent;

	UPROPERTY()
	UAnimInstanceSimple* CachedAnimInstance;

	// The weapon whose hitbox is currently open. Held instead of resolved again on close,
	// so a weapon swap or unequip mid-swing cannot leave the wrong hitbox active.
	UPROPERTY()
	TObjectPtr<AWeaponStaticBase> ActiveAttackWeapon;

	// Cleared whenever a window opens, so every swing hits a given target at most once.
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsHitThisSwing;

	AWeaponStaticBase* ResolveAttackingWeapon(EEquipmentSlot HandSlot) const;

	UFUNCTION()
	void HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	FWeaponAction ResolveWeaponAction(const FCombatInputIdentity& CombatInputIdentity) const;

	FName ResolveMontageSectionName(const FWeaponAction& AttackChain) const;
	void StartAttackChain(const FWeaponAction& Action, const FCombatInputIdentity& InputIdentity);

	FVector PreviousTraceLocation = FVector::ZeroVector;

	UPROPERTY()
	TMap<TEnumAsByte<EPhysicalSurface>, FResolvedHitFeedback> CachedSurfaceFeedbackMap;

	UPROPERTY()
	FResolvedHitFeedback CachedDefaultSurfaceFeedback;

	void CacheSurfaceFeedback();

	void PlayEnvironmentHitFeedback(const FHitResult& Hit);

	// The sustained action currently held, plus the input that started it. A copy is kept                                                                                                                                                                                                                                                                                                                                         
	// instead of resolving again on release, so a weapon swap mid-hold cannot end the wrong                                                                                                                                                                                                                                                                                                                                       
	// action — same reasoning as ActiveAttackWeapon
	UPROPERTY()
	FWeaponAction ActiveSustainedAction;

	// Slot == Undefined means no sustained action is active
	UPROPERTY()
	FCombatInputIdentity ActiveSustainedInput;

	UPROPERTY()
	TArray<FCombatInputIdentity> HeldCombatInputs;

	void BeginSustainedAction(const FWeaponAction& Action, const FCombatInputIdentity& InputIdentity);
	void EndSustainedAction();
	bool HasActiveSustainedAction() const { return ActiveSustainedInput.Slot != EEquipmentSlot::Undefined; }

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveAttackMontage;

	UPROPERTY()
	FCombatInputIdentity ActiveAttackInput;

	bool IsPerformingAttack() const;

public:
	// Called from UAnimNotifyState_Attack.
	void BeginAttackWindow(EEquipmentSlot AttackingSlot);
	void EndAttackWindow();

	void HandleWeaponHit(const FHitResult& Hit);

	void PerformWeaponAction(const FCombatInputIdentity& CombatInputIdentity);
	void ReevaluateHeldInputs();

	void PerformRightWeaponNormalAction() { PerformWeaponAction(FCombatInputIdentity{EEquipmentSlot::MainHand, false}); }
	void PerformLeftWeaponNormalAction() { PerformWeaponAction(FCombatInputIdentity{EEquipmentSlot::OffHand, false}); }
	void PerformRightWeaponHeavyAction() { PerformWeaponAction(FCombatInputIdentity{EEquipmentSlot::MainHand, true}); }
	void PerformLeftWeaponHeavyAction() { PerformWeaponAction(FCombatInputIdentity{EEquipmentSlot::OffHand, true}); }

	void OpenComboWindow() { bIsComboWindowOpen = true; }
	void CloseComboWindow() { bIsComboWindowOpen = false; }

	// Called every frame of the open attack window from UAnimNotifyState_Attack.                                                                                                                                                                                                                                                                                                                                                  
	void TickAttackTrace();

	void ReleaseWeaponAction(const FCombatInputIdentity& CombatInput);
	void CancelCurrentWeaponAction();

	void ReleaseRightWeaponNormalAction() { ReleaseWeaponAction(FCombatInputIdentity{EEquipmentSlot::MainHand, false}); }
	void ReleaseLeftWeaponNormalAction() { ReleaseWeaponAction(FCombatInputIdentity{EEquipmentSlot::OffHand, false}); }
	void ReleaseRightWeaponHeavyAction() { ReleaseWeaponAction(FCombatInputIdentity{EEquipmentSlot::MainHand, true}); }
	void ReleaseLeftWeaponHeavyAction() { ReleaseWeaponAction(FCombatInputIdentity{EEquipmentSlot::OffHand, true}); }
};

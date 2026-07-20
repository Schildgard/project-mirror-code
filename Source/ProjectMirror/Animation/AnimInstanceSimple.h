// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProjectMirror/Utility/CombatData.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "AnimInstanceSimple.generated.h"

class ACharacterBase;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UAnimInstanceSimple : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<ACharacterBase> Character = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> MovementComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> InteractionAnimMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> TakeAnimMontageLow = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> TakeAnimMontageMid = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> TakeAnimMontageHigh = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> OpenAnimMontageLow = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> OpenAnimMontageMid = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> PushAnimMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> IgniteAnimMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> RestingAnimMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> ClimbAnimMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UAnimMontage> PullFromLedgeMontage = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float FacingDirection = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EWeaponType RightHandWeapon = EWeaponType::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EWeaponType LeftHandWeapon = EWeaponType::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ECharacterAnimationState CurrentActionState = ECharacterAnimationState::Idle;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bIsSustainingAction = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ESustainedPose CurrentSustainedPose = ESustainedPose::Guard;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EEquipmentSlot SustainedSlot = EEquipmentSlot::Undefined;

public:
	void SetSustainedAction(bool bIsSustaining, ESustainedPose Pose);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsStrafing = false;

	UPROPERTY(EditAnywhere)
	float SitMontageBlendTime = 1.0f;

	bool bIsInteractionAnimPlaying = false;

	UPROPERTY(BlueprintReadOnly)
	bool bMirrorCurrentMontage = false;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	bool TryPlayInteractAnimationMontage(const EInteractionCategory InteractionCategory, const EInteractionHeight InteractionHeight = EInteractionHeight::Mid,
	                                     const EEquipmentSlot TargetsSlot = EEquipmentSlot::Undefined);
	void TryPlayRestingAnimationMontage();
	bool TryPlayClimbAnimation();
	bool TryPullFromLedgeMontage();

	void HandlePauseMenuClosed();
	void HandleOnInteractAnimationFinished(UAnimMontage* Montage, bool bInterrupted);
	void HandleOnRestingAnimationFinished(UAnimMontage* Montage, bool bInterrupted);
	void HandleOnPullUpFromLedgeAnimationFinished(UAnimMontage* Montage, bool bInterrupted);
	void HandleClimbStarted();
	void HandleClimbEnd();

	virtual void NativeBeginPlay() override;

	UFUNCTION()
	void HandleWeaponChange(EWeaponType NewWeaponType, bool bIsOffhand);

	UAnimMontage* GetPickupMontageHeightVariant(const EInteractionCategory PickUpCategory, const EInteractionHeight PickUpHeight);

	bool TryConfigPickUpMontageAndPlay(UAnimMontage* PickUpMontage, const EEquipmentSlot TargetSlot);
	ECharacterAnimationState GetCurrentActionState() const { return CurrentActionState; }
	void SetCurrentActionState(const ECharacterAnimationState NewState) { CurrentActionState = NewState; };

	bool TryPerformAttackAnimation(UAnimMontage* WeaponMontage, const FName& SectionName = NAME_None);
	bool IsAttackMontagePlaying(const UAnimMontage* WeaponMontage) const;
	bool TryChainAttackAnimation(UAnimMontage* WeaponMontage, const FName& SectionName);
	
	void SetSustainedAction(bool bIsSustaining, ESustainedPose Pose, EEquipmentSlot Slot);
};

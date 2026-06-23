// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GroundSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float FacingDirection = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;

public:
	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsStrafing = false;

	bool bIsInteractionAnimPlaying = false;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	bool TryPlayInteractAnimationMontage();
	void HandleOnInteractAnimationFinished(UAnimMontage* Montage, bool bInterrupted);
	UAnimMontage* GetInteractionAnimationMontage() const { return InteractionAnimMontage; }
};

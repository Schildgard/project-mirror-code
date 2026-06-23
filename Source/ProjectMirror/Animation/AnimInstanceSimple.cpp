// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimInstanceSimple.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectMirror/Characters/CharacterBase.h"

void UAnimInstanceSimple::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacterBase>(GetOwningActor());
	if (!IsValid(Character))
	{
		return;
	}
	MovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
	if (!IsValid(MovementComponent))
	{
		return;
	}
}

void UAnimInstanceSimple::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!IsValid(MovementComponent))
	{
		return;
	}
	Velocity = MovementComponent->Velocity;

	const FVector& HorizontalMovementSpeed = FVector{Velocity.X, Velocity.Y, 0.0f};
	GroundSpeed = HorizontalMovementSpeed.Length();
	FacingDirection = UKismetAnimationLibrary::CalculateDirection(HorizontalMovementSpeed, Character->GetActorRotation());

	bShouldMove = GroundSpeed > 3.0f;
	bIsFalling = MovementComponent->IsFalling();
}

bool UAnimInstanceSimple::TryPlayInteractAnimationMontage()
{
	if (!IsValid(InteractionAnimMontage))
	{
		return false;
	}
	if (Montage_Play(InteractionAnimMontage) > 0.0f)
	{
		bIsInteractionAnimPlaying = true;
	}
	FOnMontageBlendingOutStarted AnimEndDelegate;
	AnimEndDelegate.BindUObject(this, &UAnimInstanceSimple::HandleOnInteractAnimationFinished);
	
	Montage_SetBlendingOutDelegate(AnimEndDelegate, InteractionAnimMontage);
	return true;
}

void UAnimInstanceSimple::HandleOnInteractAnimationFinished(UAnimMontage* Montage, bool bInterrupted)
{
	bIsInteractionAnimPlaying = false;
}

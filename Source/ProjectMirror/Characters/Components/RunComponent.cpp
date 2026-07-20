// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "RunComponent.h"

#include "StaminaComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectMirror/Animation/AnimInstanceSimple.h"
#include "ProjectMirror/Settings/SettingsCharacter.h"
#include "ProjectMirror/Utility/EnumCollection.h"


URunComponent::URunComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URunComponent::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	if (!IsValid(OwningActor))
	{
		return;
	}
	const ACharacter* OwningCharacter = Cast<ACharacter>(OwningActor);
	if (!IsValid(OwningCharacter))
	{
		return;
	}
	const USkeletalMeshComponent* SkeletalMeshComponent = OwningCharacter->GetMesh();
	if (!IsValid(SkeletalMeshComponent))
	{
		return;
	}

	CachedAnimInstance = Cast<UAnimInstanceSimple>(SkeletalMeshComponent->GetAnimInstance());
	CachedMovementComponent = Cast<UCharacterMovementComponent>(OwningCharacter->GetMovementComponent());
}

void URunComponent::StartRunning()
{
	if (bIsRunning)
	{
		return;
	}
	
	if (IsValid(CachedStaminaComponent))
	{
		if (CachedStaminaComponent->IsExhausted())
		{
			return;
		}
	}

	//Check Conditions for Running
	if (IsValid(CachedAnimInstance) && CachedAnimInstance->bShouldMove)
	{
		ECharacterAnimationState CurrentAnimationState = CachedAnimInstance->GetCurrentActionState();

		if (CurrentAnimationState == ECharacterAnimationState::Idle)
		{
			if (const USettingsCharacter* CharacterSettings = GetDefault<USettingsCharacter>())
			{
				if (CachedMovementComponent)
				{
					CachedMovementComponent->MaxWalkSpeed = CharacterSettings->DefaultRunningSpeed;
				}
			}
			CachedAnimInstance->SetCurrentActionState(ECharacterAnimationState::Running);
			OnRunStarted.Broadcast();
			bIsRunning = true;
		}
	}
}

void URunComponent::CancelRunning()
{
	if (!bIsRunning)
	{
		return;
	}
	if (IsValid(CachedAnimInstance))
	{
		CachedAnimInstance->SetCurrentActionState(ECharacterAnimationState::Idle);
		//If Running is interrupted by an Attack or other Animation, a Notify in the Animation must update the AnimaationState
	}
	if (IsValid(CachedMovementComponent))
	{
		if (const USettingsCharacter* CharacterSettings = GetDefault<USettingsCharacter>())
		{
			CachedMovementComponent->MaxWalkSpeed = CharacterSettings->DefaultWalkingSpeed;
		}
	}
	bIsRunning = false;
	OnRunEnded.Broadcast();
}

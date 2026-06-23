// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "LockOnComponent.h"
#include "Characters/CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/Focusable.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ValidObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
}


void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
	CachedCameraArmComponent = GetOwner()->FindComponentByClass<USpringArmComponent>();

	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwningPawn))
	{
		return;
	}

	APlayerController* OwningController = Cast<APlayerController>(OwningPawn->GetController());
	if (!IsValid(OwningController))
	{
		return;
	}
	CachedPlayerController = OwningController;
}


void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(FocusedTarget))
	{
		if (IsValid(CachedPlayerController))
		{
			FVector CameraArmPos;
			if (bUseCameraPos && IsValid(CachedCameraComponent))
			{
				CameraArmPos = CachedCameraComponent->GetComponentLocation();
			}
			else if (IsValid(CachedCameraArmComponent))
			{
				CameraArmPos = CachedCameraArmComponent->GetComponentLocation();
			}
			else
			{
				return;
			}
			const FVector& TargetPosition = FocusedTarget->GetActorLocation();
			//todo: replace Actor Location with Socket Location if available

			const FRotator FocusRotation = UKismetMathLibrary::FindLookAtRotation(CameraArmPos, TargetPosition);
			const FRotator InterpolatedRotation = FMath::RInterpTo(CachedPlayerController->GetControlRotation(),FocusRotation, DeltaTime,10.f); //@todo change the speed in settings
			CachedPlayerController->SetControlRotation(InterpolatedRotation);

			GetOwner()->SetActorRotation(FRotator(0.0f, InterpolatedRotation.Yaw, 0.0f));
		}
	}
}

void ULockOnComponent::FocusOnTarget(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	FocusedTarget = TargetActor;
	
	IFocusable::Execute_OnFocused(FocusedTarget);
}


void ULockOnComponent::ToggleLockOn()
{
	ACharacterBase* Character = Cast<ACharacterBase>(GetOwner());
	//If Lock On is already active, deactivate LockOn and return
	if (IsValid(FocusedTarget))
	{
		IFocusable::Execute_OnFocusLost(FocusedTarget);
		FocusedTarget = nullptr;
		if (IsValid(Character))
		{
			Character->SetStrafing(false);
		}
		return;
	}
	//Else Lock to closest target
	float ClosestDistance = FLT_MAX;
	AActor* ClosestValidTarget = UGameplayStatics::FindNearestActor(GetOwner()->GetActorLocation(),
	                                                                ScanForValidTargets(), ClosestDistance);
	if (IsValid(ClosestValidTarget))
	{
		FocusOnTarget(ClosestValidTarget);
		if (IsValid(Character))
		{
			Character->SetStrafing(true);
		}
	}
}

TArray<AActor*> ULockOnComponent::ScanForValidTargets() const
{
	const FVector& PlayerPosition = GetOwner()->GetActorLocation();
	TArray<FHitResult> ScanHits;

	UKismetSystemLibrary::SphereTraceMultiForObjects(this, PlayerPosition, PlayerPosition, 750.0f, ValidObjectTypes,
	                                                 false, TArray<AActor*>(), EDrawDebugTrace::None, ScanHits, true,
	                                                 FLinearColor::Red, FLinearColor::Green, 5.0f);

	if (ScanHits.IsEmpty())
	{
		return TArray<AActor*>();
	}

	TArray<AActor*> ValidTargets;
	ValidTargets.Reserve(ScanHits.Num());

	for (const FHitResult& Hit : ScanHits)
	{
		AActor* HitActor = Hit.GetActor();

		if (!IsValid(HitActor) || ValidTargets.Contains(HitActor))
		{
			continue;
		}
		if (HitActor->Implements<UFocusable>())
		{
			ValidTargets.Add(HitActor);
		}
	}

	return ValidTargets;
}

void ULockOnComponent::SwitchLockOnTarget(const FInputActionValue& InputValue)
{
	if (!IsValid(FocusedTarget))
	{
		return;
	}

	TArray<AActor*> ValidTargets = ScanForValidTargets();

	AActor* ClosestActor = nullptr;
	float ClosestDistance = FLT_MAX;

	float DirectionValue = InputValue.Get<float>();

	for (AActor* Actor : ValidTargets)
	{
		if (Actor == FocusedTarget)
		{
			continue;
		}
		const FVector& RelativeLocation = GetOwner()->GetTransform().
		                                              InverseTransformPosition(Actor->GetActorLocation());
		if (DirectionValue > 0.0f && RelativeLocation.Y > 0.0f && RelativeLocation.X > 0.0f)
		{
			float CurrentDistance = FMath::Abs(RelativeLocation.Y);
			if (CurrentDistance < ClosestDistance)
			{
				ClosestDistance = CurrentDistance;
				ClosestActor = Actor;
			}
		}
		else if (DirectionValue < 0.0f && RelativeLocation.Y < 0.0f&& RelativeLocation.X > 0.0f)
		{
			float CurrentDistance = FMath::Abs(RelativeLocation.Y);
			if (CurrentDistance < ClosestDistance)
			{
				ClosestDistance = CurrentDistance;
				ClosestActor = Actor;
			}
		}
	}

	if (IsValid(ClosestActor))
	{
		FocusOnTarget(ClosestActor);
	}
}

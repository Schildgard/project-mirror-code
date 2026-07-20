// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "ClimbComponent.h"

#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectMirror/Animation/AnimInstanceSimple.h"
#include "ProjectMirror/Settings/SettingsCharacter.h"

UClimbComponent::UClimbComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UClimbComponent::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !IsValid(World))
	{
		return;
	}

	CachedOwnerCharacter = Cast<ACharacter>(Owner);
	if (CachedOwnerCharacter)
	{
		CachedOwnerMesh = CachedOwnerCharacter->GetMesh();
		CachedMovementComponent = Cast<UCharacterMovementComponent>(CachedOwnerCharacter->GetMovementComponent());
	}
}

void UClimbComponent::HangOnClimbable()
{
	if (CurrentClimbingState != EClimbingState::None)
	{
		return;
	}

	FClimbData InClimbData;
	CheckClimbableDistance(InClimbData);
	if (!InClimbData.bClimbable)
	{
		return;
	}
	CheckClimbableHeight(InClimbData);
	if (!InClimbData.bClimbable)
	{
		return;
	}
	CachedClimbData = InClimbData;
	TryPlayClimbAnimation();
}

void UClimbComponent::CheckClimbableDistance(FClimbData& OutClimbData)
{
	if (!IsValid(CachedOwnerCharacter))
	{
		OutClimbData.bClimbable = false;
		return;
	}
	const USettingsCharacter* CharSettings = GetDefault<USettingsCharacter>();
	const FVector OwnerLocation = CachedOwnerCharacter->GetActorLocation();
	FVector CharacterForwardDirection = CachedOwnerCharacter->GetActorForwardVector();
	const FVector DestinationDirection = CharacterForwardDirection * CharSettings->AcceptableForwardDistance;
	const FVector ScanDestination = OwnerLocation + DestinationDirection;

	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), OwnerLocation, ScanDestination, CharSettings->ClimbScanRadius,
	                                                  {CharSettings->ClimbableObjectType}, true,
	                                                  TArray<AActor*>{CachedOwnerCharacter},
	                                                  EDrawDebugTrace::ForDuration, HitResult, true);

	if (HitResult.IsValidBlockingHit())
	{
		AActor* HitActor = HitResult.GetActor();
		if (!IsValid(HitActor))
		{
			return;
		}
		OutClimbData.ClimbLocation = HitResult.ImpactPoint;
		OutClimbData.ClimbNormal = HitResult.ImpactNormal;
		OutClimbData.ClimbableActor = HitActor;
		OutClimbData.bClimbable = true;
	}
}

void UClimbComponent::CheckClimbableHeight(FClimbData& OutClimbData)
{
	if (!IsValid(CachedOwnerCharacter))
	{
		OutClimbData.bClimbable = false;
		return;
	}
	const USettingsCharacter* CharSettings = GetDefault<USettingsCharacter>();

	const FVector ForwardHit = OutClimbData.ClimbLocation;
	FVector InwardDirection = -OutClimbData.ClimbNormal;
	InwardDirection.Z = 0.0f;
	InwardDirection.Normalize();
	const FVector LedgeProbe = ForwardHit + InwardDirection * CharSettings->EdgeInwardOffset;
	const FVector ScanLocation = FVector(LedgeProbe.X, LedgeProbe.Y, ForwardHit.Z + CharSettings->AcceptableUpwardDistance);
	const FVector ScanDestination = FVector(LedgeProbe.X, LedgeProbe.Y, ForwardHit.Z);


	FHitResult UpwardHitResult;
	UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), ScanLocation, ScanDestination, CharSettings->ClimbScanRadius,
	                                                  {CharSettings->ClimbableObjectType}, true,
	                                                  TArray<AActor*>{CachedOwnerCharacter},
	                                                  EDrawDebugTrace::ForDuration, UpwardHitResult, true);

	if (UpwardHitResult.IsValidBlockingHit())
	{
		AActor* HitActor = UpwardHitResult.GetActor();
		if (!IsValid(HitActor))
		{
			OutClimbData.bClimbable = false;
			return;
		}

		if (UpwardHitResult.ImpactNormal.Z < CharSettings->MinLedgeSurfaceUpDot)
		{
			OutClimbData.bClimbable = false;
			return;
		}
		const float LedgeTopZ = UpwardHitResult.ImpactPoint.Z;

		const float HeightDistance = LedgeTopZ - CachedOwnerCharacter->GetActorLocation().Z;
		if (HeightDistance >= CharSettings->MinHeightToleranceBetweenSocketAndEdge && HeightDistance <= CharSettings->MaxHeightToleranceBetweenSocketAndEdge)
		{
			OutClimbData.ClimbLocation = FVector(ForwardHit.X, ForwardHit.Y, LedgeTopZ);
			return;
		}
	}
	OutClimbData.bClimbable = false;
}

void UClimbComponent::SnapOnClimbable()
{
	const FClimbData& InClimbData = CachedClimbData;

	if (!IsValid(CachedMovementComponent) || !IsValid(CachedOwnerMesh) || !IsValid(CachedOwnerCharacter))
	{
		return;
	}
	const USettingsCharacter* CharSettings = GetDefault<USettingsCharacter>();
	CachedMovementComponent->StopMovementImmediately();
	CachedMovementComponent->SetMovementMode(MOVE_Flying);

	FVector FlatWallNormal = InClimbData.ClimbNormal;
	FlatWallNormal.Z = 0.0f;
	FlatWallNormal.Normalize();
	const FRotator SnapRotation = (-FlatWallNormal).Rotation();

	CachedOwnerCharacter->AttachToActor(InClimbData.ClimbableActor, FAttachmentTransformRules::KeepWorldTransform);
	CachedOwnerCharacter->SetActorRotation(SnapRotation);

	const FVector HangTarget = InClimbData.ClimbLocation + (FlatWallNormal * CharSettings->HangBackDistance) - (FVector::UpVector * CharSettings->
		HangDownDistance);
	CachedOwnerCharacter->SetActorLocation(HangTarget);
	OnClimbStart.Broadcast();
}

void UClimbComponent::SnapOnLedgeSurface()
{
	const FClimbData& InClimbData = CachedClimbData;

	if (!IsValid(CachedMovementComponent) || !IsValid(CachedOwnerMesh) || !IsValid(CachedOwnerCharacter))
	{
		return;
	}
	const USettingsCharacter* CharSettings = GetDefault<USettingsCharacter>();

	FVector InwardDirection = -InClimbData.ClimbNormal;
	InwardDirection.Z = 0.0f;
	InwardDirection.Normalize();

	const FVector SnapLocation = InClimbData.ClimbLocation + InwardDirection * CharSettings->SnapOnLedgeForwardOffset + FVector::UpVector * CharSettings->
		SnapOnLedgeUpwardOffset;
	CachedOwnerCharacter->SetActorLocation(SnapLocation);

	CompleteClimbing();
}

void UClimbComponent::TryPlayClimbAnimation()
{
	if (!IsValid(CachedOwnerMesh) || CurrentClimbingState != EClimbingState::None)
	{
		return;
	}
	CurrentClimbingState = EClimbingState::Hanging;

	UAnimInstanceSimple* SimpleAnimInstance = Cast<UAnimInstanceSimple>(CachedOwnerMesh->GetAnimInstance());
	if (!IsValid(SimpleAnimInstance) || !SimpleAnimInstance->TryPlayClimbAnimation())
	{
		SnapOnClimbable();
	}
}

void UClimbComponent::PullUpFromLedge()
{
	if (!IsValid(CachedOwnerMesh) || CurrentClimbingState != EClimbingState::Hanging)
	{
		return;
	}
	CurrentClimbingState = EClimbingState::PullingUp;

	UAnimInstanceSimple* SimpleAnimInstance = Cast<UAnimInstanceSimple>(CachedOwnerMesh->GetAnimInstance());
	if (!IsValid(SimpleAnimInstance) || !SimpleAnimInstance->TryPullFromLedgeMontage())
	{
		SnapOnLedgeSurface();
	}
}

void UClimbComponent::HandleClimbingInput(const FInputActionValue& Input)
{
	const FVector2D InputDirection = Input.Get<FVector2D>();

	float HorizontalInput = InputDirection.X;
	float VerticalInput = InputDirection.Y;

	if (VerticalInput >= 0.5f)
	{
		PullUpFromLedge();
		return;
	}
	if (VerticalInput <= -0.5f)
	{
		CancelClimbing();
		return;
	}
	if (HorizontalInput >= 0.5f)
	{
		//Move Right
		return;
	}
	if (HorizontalInput <= -0.5f)
	{
		//Move Left
		return;
	}
}

void UClimbComponent::CompleteClimbing()
{
	if (CurrentClimbingState == EClimbingState::None)
	{
		return;
	}
	if (IsValid(CachedOwnerCharacter))
	{
		CachedOwnerCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	if (IsValid(CachedMovementComponent))
	{
		CachedMovementComponent->SetMovementMode(MOVE_Walking);
	}

	CurrentClimbingState = EClimbingState::None;
	OnClimbEnd.Broadcast();
}

void UClimbComponent::CancelClimbing()
{
	if (CurrentClimbingState != EClimbingState::Hanging)
	{
		return;
	}

	CompleteClimbing();

	if (!IsValid(CachedMovementComponent))
	{
		return;
	}

	CachedMovementComponent->SetMovementMode(MOVE_Falling);
}

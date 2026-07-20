// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractableMysteriousMirror.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SphereComponent.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"
#include "ProjectMirror/Utility/Utilities.h"


// Sets default values
AInteractableMysteriousMirror::AInteractableMysteriousMirror()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionCategory = EInteractionCategory::Undefined;
	bIsOneTimeInteraction = false;
	bIsInteractionPossible = false; //Locks Interaction through PlayerActionInput

	MirrorSpawnRadius = CreateDefaultSubobject<USphereComponent>(TEXT("MirrorSpawnRadius"));
	MirrorSpawnRadius->SetupAttachment(RootComponent);
	MirrorDespawnRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("DespawnRadius"));
	MirrorDespawnRadius->SetupAttachment(RootComponent);

	MirrorPlaneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Mirror Plane Component"));
	MirrorPlaneComponent->SetupAttachment(RootComponent);

	ReflectiveComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Reflection Capture Component"));
	ReflectiveComponent->SetupAttachment(RootComponent);
	ReflectiveComponent->bCaptureEveryFrame = false;
	ReflectiveComponent->bCaptureOnMovement = false;
	ReflectiveComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;

	StaticMeshComponent->SetCollisionObjectType(ECC_GameTraceChannel1);

	EventID = ActivateEventID;

	bIsPlayerFacingTowardsMirror = false;

	BIsMirroredObjectVisible = false;

	bIsPlayerInInnerRadius = false;

	bIsPlayerInOuterRadius = false;

	bIsPlayerInRange = false;
}

void AInteractableMysteriousMirror::BeginPlay()
{
	Super::BeginPlay();
	MirrorSpawnRadius->OnComponentBeginOverlap.AddUniqueDynamic(this, &AInteractableMysteriousMirror::RegisterInnerRadiusEnter);
	MirrorSpawnRadius->OnComponentEndOverlap.AddUniqueDynamic(this, &AInteractableMysteriousMirror::RegisterInnerRadiusExit);
	MirrorDespawnRadius->OnComponentBeginOverlap.AddUniqueDynamic(this, &AInteractableMysteriousMirror::RegisterOuterRadiusEnter);
	MirrorDespawnRadius->OnComponentEndOverlap.AddUniqueDynamic(this, &AInteractableMysteriousMirror::RegisterOuterRadiusExit);

	if (IsValid(ReflectiveComponent) && IsValid(MirrorPlaneComponent))
	{
		ReflectiveComponent->HideActorComponents(this);
		ReflectiveComponent->bEnableClipPlane = true;
		ReflectiveComponent->ClipPlaneBase = MirrorPlaneComponent->GetComponentLocation();
		ReflectiveComponent->ClipPlaneNormal = MirrorPlaneComponent->GetForwardVector();
	}
}

void AInteractableMysteriousMirror::RegisterInnerRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || !ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		return;
	}
	bIsPlayerInInnerRadius = true;
	CachedPlayerActor = OtherActor;
}

void AInteractableMysteriousMirror::RegisterOuterRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || !ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		return;
	}
	bIsPlayerInOuterRadius = true;
}

void AInteractableMysteriousMirror::RegisterInnerRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                            int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor) || !ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		return;
	}
	bIsPlayerInInnerRadius = false;
}

void AInteractableMysteriousMirror::RegisterOuterRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                            int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor) || !ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		return;
	}
	bIsPlayerInOuterRadius = false;

	if (CachedPlayerActor == OtherActor)
	{
		CachedPlayerActor = nullptr;
	}
}

void AInteractableMysteriousMirror::UpdateReflection()
{
	if (!IsValid(MirrorPlaneComponent) || !IsValid(ReflectiveComponent))
	{
		return;
	}

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!IsValid(CameraManager))
	{
		return;
	}

	//The mirror is a static object and can not move. So the Normal and Position are always the same. Probably no need to get this in update.
	const FVector MirrorPlanePosition = MirrorPlaneComponent->GetComponentLocation();
	const FVector MirrorPlaneNormal = MirrorPlaneComponent->GetForwardVector();

	// --- Reflect camera position across the plane ---                                                                                                                                                                                                                                                                                                                                                                      
	const FVector CameraLocation = CameraManager->GetCameraLocation();
	const float SignedDistance = FVector::DotProduct(CameraLocation - MirrorPlanePosition, MirrorPlaneNormal);
	const FVector ReflectedLocation = CameraLocation - 2.0f * SignedDistance * MirrorPlaneNormal;

	// --- Reflect camera orientation across the plane ---                                                                                                                                                                                                                                                                                                                                                                   
	const FRotator CamRotation = CameraManager->GetCameraRotation();
	const FVector CamForward = CamRotation.Vector();
	const FVector CamUp = FRotationMatrix(CamRotation).GetUnitAxis(EAxis::Z);

	// MirrorByVector(N) == V - 2*(V·N)*N  → reflection of a direction across the plane.                                                                                                                                                                                                                                                                                                                                     
	const FVector MirroredForward = CamForward.MirrorByVector(MirrorPlaneNormal);
	const FVector MirroredUp = CamUp.MirrorByVector(MirrorPlaneNormal);
	const FRotator ReflectedRotation = FRotationMatrix::MakeFromXZ(MirroredForward, MirroredUp).Rotator();

	ReflectiveComponent->SetWorldLocationAndRotation(ReflectedLocation, ReflectedRotation);

	// Match the main camera FOV, otherwise the parallax is wrong.                                                                                                                                                                                                                                                                                                                                                           
	ReflectiveComponent->FOVAngle = CameraManager->GetFOVAngle();
}


void AInteractableMysteriousMirror::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(StaticMeshComponent) && StaticMeshComponent->WasRecentlyRendered(0.1f) && IsValid(ReflectiveComponent))
	{
		UpdateReflection();
		ReflectiveComponent->CaptureScene();
	}
	bIsPlayerInRange = bIsPlayerInInnerRadius || bIsPlayerInOuterRadius;

	if (bIsPlayerInRange)
	{
		bIsPlayerFacingTowardsMirror = CheckPlayerFacingTowardsMirror();

		if (!BIsMirroredObjectVisible)
		{
			if (bIsPlayerInInnerRadius && bIsPlayerFacingTowardsMirror)
			{
				TriggerActivateEvent();
			}
		}

		else if ((!bIsPlayerInOuterRadius && !bIsPlayerInInnerRadius) || !bIsPlayerFacingTowardsMirror)
		{
			TriggerDeactivateEvent();
		}
	}
}

bool AInteractableMysteriousMirror::CheckPlayerFacingTowardsMirror()
{
	if (IsValid(CachedPlayerActor))
	{
		//ActorLookDirection must negative to MirrorLookDirection
		FVector ActorLookDirection = CachedPlayerActor->GetActorForwardVector();
		FVector MirrorLookDirection = GetActorForwardVector();
		float DotProduct = FVector::DotProduct(MirrorLookDirection, ActorLookDirection);
		if (DotProduct <= -0.5f && DotProduct >= -1.0f)
		{
			return true;
		}
	}
	return false;
}

void AInteractableMysteriousMirror::TriggerActivateEvent()
{
	if (USubsystemObjectEvents* SubsystemObjectEvents = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		SubsystemObjectEvents->FireObjectEvent(EventID);
	}
	BIsMirroredObjectVisible = true;
	EventID = DeactivateEventID;
}


void AInteractableMysteriousMirror::TriggerDeactivateEvent()
{
	BIsMirroredObjectVisible = false;
	if (USubsystemObjectEvents* SubsystemObjectEvents = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		SubsystemObjectEvents->FireObjectEvent(EventID);
	}
	EventID = ActivateEventID;
}

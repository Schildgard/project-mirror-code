// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractableMysteriousMirror.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
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
	
	StaticMeshComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
	
}

// Called when the game starts or when spawned
void AInteractableMysteriousMirror::BeginPlay()
{
	Super::BeginPlay();
	MirrorSpawnRadius->OnComponentBeginOverlap.AddUniqueDynamic(this, &AInteractableMysteriousMirror::RegisterRadiusEnter);
	MirrorDespawnRadius->OnComponentEndOverlap.AddUniqueDynamic(this, &AInteractableMysteriousMirror::RegisterRadiusExit);
}

void AInteractableMysteriousMirror::RegisterRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || !ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		return;
	}
	bIsPlayerInRegisterRadius = true;
	CachedPlayerActor = OtherActor;
	
}

void AInteractableMysteriousMirror::RegisterRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor) || !ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		return;
	}
	bIsPlayerInRegisterRadius = false;
	if (CachedPlayerActor == OtherActor)
	{
	CachedPlayerActor = nullptr;
	}
}


void AInteractableMysteriousMirror::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsPlayerInRegisterRadius)
	{
		bIsPlayerFacingTowardsMirror = CheckPlayerFacingTowardsMirror();
	}
	
	if (!bIsStaircaseVisible)
	{
		if (bIsPlayerInRegisterRadius && bIsPlayerFacingTowardsMirror)
		{
			ShowStaircase();
		}
	}
	else
	{
		if (!bIsPlayerFacingTowardsMirror || !bIsPlayerInRegisterRadius)
		{
			HideStaircase();
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

void AInteractableMysteriousMirror::ShowStaircase()
{
	OnInteract_Implementation();
	bIsStaircaseVisible = true;
	EventID = TEXT("OnMirrorFocusLost");
}

void AInteractableMysteriousMirror::HideStaircase()
{
	bIsStaircaseVisible = false;
	OnInteract_Implementation();
	EventID = TEXT("OnMirrorFocus");
}


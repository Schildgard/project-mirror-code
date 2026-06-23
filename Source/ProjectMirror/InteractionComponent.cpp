// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractionComponent.h"

#include "Animation/AnimInstanceSimple.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Settings/SettingsCharacter.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	InteractionSphereComponent = CreateDefaultSubobject<USphereComponent>("InteractionSphereComponent");
	InteractionSphereComponent->SetupAttachment(this);
	if (const USettingsCharacter* Settings = GetDefault<USettingsCharacter>())
	{
		InteractionSphereComponent->SetSphereRadius(Settings->InteractionRadius);
	}
}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	InteractionSphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UInteractionComponent::RegisterRadiusEnter);
	InteractionSphereComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &UInteractionComponent::RegisterRadiusExit);
}

void UInteractionComponent::RegisterRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || !OtherActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		return;
	}
	if (!IInteractable::Execute_IsInterActionPossible(OtherActor))
	{
		return;
	}
	if (IInteractable* InteractableInterface = Cast<IInteractable>(OtherActor))
	{
		InteractableInterface->GetOnInteractionDisabledDelegate().AddUniqueDynamic(this, &UInteractionComponent::RemoveInteractable);
	}
	AddInteractable(OtherActor);
	OnRadiusEnter.Broadcast(OtherActor);
}

void UInteractionComponent::RegisterRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                               int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor) || !OtherActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		return;
	}
	RemoveInteractable(OtherActor);
	OnRadiusExit.Broadcast(OtherActor);
}

void UInteractionComponent::Interact()
{
	if (!IsValid(CurrentInteractable))
	{
		return;
	}
	if (IInteractable::Execute_IsInterActionPossible(CurrentInteractable))
	{
		IInteractable::Execute_OnInteract(CurrentInteractable);
	}
}

void UInteractionComponent::AddInteractable(AActor* InteractableActor)
{
	InteractablesInRange.AddUnique(InteractableActor);
	if (!CurrentInteractable)
	{
		CurrentInteractable = InteractableActor;
		OnCurrentInteractableChanged.Broadcast(CurrentInteractable);
	}
}

void UInteractionComponent::RemoveInteractable(AActor* InteractableActor)
{
	InteractablesInRange.Remove(InteractableActor);
	if (IInteractable* InteractableInterface = Cast<IInteractable>(InteractableActor))
	{
		InteractableInterface->GetOnInteractionDisabledDelegate().RemoveAll(this);
	}
	if (CurrentInteractable == InteractableActor && IsValid(GetOwner()))
	{
		float Distance = FLT_MAX;
		CurrentInteractable = InteractablesInRange.IsEmpty()
			                      ? nullptr
			                      : UGameplayStatics::FindNearestActor(GetOwner()->GetActorLocation(), InteractablesInRange, Distance);
		if (CurrentInteractable)
		{
			UE_LOG(LogInteraction, Display, TEXT("%s is current Interactable"), *CurrentInteractable->GetFName().ToString())
		}
		OnCurrentInteractableChanged.Broadcast(CurrentInteractable);
	}
}

EInteractionCategory UInteractionComponent::GetCurrentInteractionType() const
{
	if (!IsValid(CurrentInteractable))
	{
		return EInteractionCategory::Undefined;
	}
	return IInteractable::Execute_GetInteractionCategory(CurrentInteractable);
}

void UInteractionComponent::SnapActorToCurrentInteractable()
{
	AActor* Owner = GetOwner();
	USceneComponent* SnapTargetComponent = IInteractable::Execute_GetInteractionSnapTargetComponent(CurrentInteractable);
	
	if (!IsValid(SnapTargetComponent))
	{
		return;
	}
	const FVector SnapLocation = SnapTargetComponent->GetComponentLocation(); 
	const FRotator LookAt  = UKismetMathLibrary::FindLookAtRotation(SnapLocation, CurrentInteractable->GetActorLocation());
	const FRotator TargetRotation = FRotator{0.f, LookAt.Yaw, 0.f};
	Owner->SetActorRotation(TargetRotation);
	Owner->SetActorLocation(SnapLocation, false, nullptr, ETeleportType::TeleportPhysics);
	
}

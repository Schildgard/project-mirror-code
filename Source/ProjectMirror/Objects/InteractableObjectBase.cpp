// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractableObjectBase.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"

AInteractableObjectBase::AInteractableObjectBase()
{
	PrimaryActorTick.bCanEverTick = false;
	SnapTargetComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SnapTargetComponent"));
}

void AInteractableObjectBase::BeginPlay()
{
	Super::BeginPlay();

	if (bIsOneTimeInteraction && !EventID.IsNone())
	{
		if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
		{
			if (EventSubsystem->IsObjectActivated(EventID))
			{
				bIsInteractionPossible = false;
			}
		}
	}
}

void AInteractableObjectBase::OnInteract_Implementation(UInteractionComponent* InteractingComponent)
{
	if (EventID.IsNone())
	{
		return;
	}

	UE_LOG(LogInteraction, Display, TEXT("%s gets Interacted with"), *GetFName().ToString());


	if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		if (bIsOneTimeInteraction)
		{
			bIsInteractionPossible = false;
			OnInteractionDisabled.Broadcast(this);
		}
		EventSubsystem->UpdateObjectStatusInActivatedObjects(EventID);
		EventSubsystem->FireObjectEvent(EventID);
	}
}

FName AInteractableObjectBase::GetInteractionPrompt_Implementation()
{
	//@todo: think about if this has to be in an interface. Maybe making this an utility function would make more sense?
	switch (InteractionCategory)
	{
	case EInteractionCategory::Talk:
		return TEXT("Press E to Talk");
	case EInteractionCategory::Interact:
		return TEXT("Press E to Interact");
	case EInteractionCategory::Take:
		return TEXT("Press E to Take");
	case EInteractionCategory::Push:
		return TEXT("Press E to Push");
	case EInteractionCategory::Open:
		return TEXT("Press E to Open");
	case EInteractionCategory::Close:
		return TEXT("Press E to Close");
	case EInteractionCategory::Ignite:
		return TEXT("Press E to Ignite");
	default: return NAME_None;
	}
}

bool AInteractableObjectBase::IsInterActionPossible_Implementation(UInteractionComponent* InteractionComponent)
{
	return bIsInteractionPossible;
}

EInteractionCategory AInteractableObjectBase::GetInteractionCategory_Implementation()
{
return InteractionCategory;
}

USceneComponent* AInteractableObjectBase::GetInteractionSnapTargetComponent_Implementation()
{
	return SnapTargetComponent;
}

FOnInteractionDisabled& AInteractableObjectBase::GetOnInteractionDisabledDelegate()
{
	return OnInteractionDisabled;
}

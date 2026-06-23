// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "TriggerableObject.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"
#include "ProjectMirror/Utility/EnumCollection.h"

ATriggerableObject::ATriggerableObject()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	RootComponent = MeshComponent;
}

void ATriggerableObject::BeginPlay()
{
	Super::BeginPlay();

	if (EventMap.IsEmpty())
	{
		UE_LOG(LogTriggerableObjects, Warning, TEXT("has no events in Event map. %s wont react"), *GetFName().ToString())
		return;
	}

	USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>();
	if (!IsValid(EventSubsystem))
	{
		return;
	}

	for (const TPair<FName, FEventReactionConfig>& Event : EventMap)
	{
		if (!Event.Value.bCheckObjectConditionOnBeginPlay && !Event.Value.bRegisterOnEvent)
		{
			UE_LOG(LogTriggerableObjects, Warning, TEXT("%s: Event '%s' has both flags false — entry will never react."), *GetName(), *Event.Key.ToString())
			continue;
		}
		if (Event.Value.bCheckObjectConditionOnBeginPlay)
		{
			if (EventSubsystem->IsObjectActivated(Event.Key))
			{
				Execute_OnTriggered(this, Event.Value.EventReaction);
			}
		}

		if (Event.Value.bRegisterOnEvent)
		{
			if (FSimpleMulticastDelegate* ObjectEvent = EventSubsystem->FindOrRegisterObjectEventListener(Event.Key))
			{
				ObjectEvent->AddWeakLambda(this, [this, ReactionType = Event.Value.EventReaction]()
				{
					Execute_OnTriggered(this, ReactionType);
				});
			}
		}
	}
}

void ATriggerableObject::OnTriggered_Implementation(const EEventReactionType EventReaction)
{
	switch (EventReaction)
	{
	case EEventReactionType::BecomeVisible:
		MeshComponent->SetVisibility(true, true);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		break;
	case EEventReactionType::BecomeHidden:
		MeshComponent->SetVisibility(false, true);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EEventReactionType::ToggleVisibility:
		{
			bool NewVisibility = !MeshComponent->GetVisibleFlag();
			bool CollisionActive = MeshComponent->GetCollisionEnabled() == ECollisionEnabled::QueryAndPhysics;
			MeshComponent->SetVisibility(NewVisibility, true);
			if (CollisionActive)
			{
				MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else
			{
				MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
		}
		break;
	case EEventReactionType::ChangeScale:
		SetActorScale3D(FVector(20.f));
		break;
	case EEventReactionType::ActivateChildObject:
		//@todo currently not relevant. keep this for later.
		break;
	default:
		break;
	}
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractableHealingDwell.h"

#include "ProjectMirror/Characters/CharacterBase.h"
#include "ProjectMirror/Characters/Components/AttributeComponent.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"


AInteractableHealingDwell::AInteractableHealingDwell()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionCategory = EInteractionCategory::Undefined;
	bIsOneTimeInteraction = false;
}

void AInteractableHealingDwell::BeginPlay()
{
	Super::BeginPlay();
	RemainingCapacity = InitialCapacity;

	if (EventID.IsNone())
	{
		UE_LOG(LogInteraction, Warning, TEXT("%s has no EventID. Fill level will not persist."), *GetFName().ToString());
		return;
	}
	if (const USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		if (EventSubsystem->HasFloatState(EventID))
		{
			RemainingCapacity = EventSubsystem->GetFloatState(EventID);
		}
	}

	bIsEmpty = RemainingCapacity <= 0.0f;
}

void AInteractableHealingDwell::TransferHealth(UAttributeComponent* AttributeComponent)
{
	if (!IsValid(AttributeComponent) || AttributeComponent->IsAtMaxHealth() || bIsEmpty)
	{
		return;
	}
	const float HealAmount = FMath::Min(HealingRate * GetWorld()->GetDeltaSeconds(), RemainingCapacity);
	const float HealedAmount = AttributeComponent->IncreaseCurrentHealth(HealAmount);

	RemainingCapacity -= HealedAmount;

	if (!EventID.IsNone())
	{
		if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
		{
			EventSubsystem->SetFloatState(EventID, RemainingCapacity);
		}
	}

	if (RemainingCapacity <= 0.0f)
	{
		bIsEmpty = true;
		OnHealingDwellEmpty.Broadcast();
	}
}

void AInteractableHealingDwell::OnInteract_Implementation(UInteractionComponent* InteractionComponent)
{
	if (!IsValid(InteractionComponent))
	{
		return;
	}

	const AActor* InteractingActor = InteractionComponent->GetOwner();

	if (!IsValid(InteractingActor))
	{
		return;
	}

	const ACharacterBase* InteractingCharacter = Cast<ACharacterBase>(InteractingActor);

	if (!IsValid(InteractingCharacter))
	{
		return;
	}

	UAttributeComponent* AttributeComponent = InteractingCharacter->GetAttributeComponent();
	if (!IsValid(AttributeComponent))
	{
		return;
	}

	TransferHealth(AttributeComponent);
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectStatic.h"
#include "InteractableHealingDwell.generated.h"

class UAttributeComponent;

DECLARE_MULTICAST_DELEGATE(FOnHealingDwellEmpty);
UCLASS()
class PROJECTMIRROR_API AInteractableHealingDwell : public AInteractableObjectStatic
{
	GENERATED_BODY()

protected:
	AInteractableHealingDwell();

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float HealCapacity = 2000.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float InitialCapacity = 2000.f;

	UPROPERTY(VisibleAnywhere)
	float RemainingCapacity = 0.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float HealingRate = 100.f;

	bool bIsEmpty = false;

	virtual void BeginPlay() override;

public:
	FOnHealingDwellEmpty OnHealingDwellEmpty;

	void TransferHealth(UAttributeComponent* AttributeComponent);
	virtual void OnInteract_Implementation(UInteractionComponent* InteractionComponent) override;
};

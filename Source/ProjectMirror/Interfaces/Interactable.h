// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "Components/SceneComponent.h"
#include "ProjectMirror/InteractionComponent.h"
#include "ProjectMirror/Data/DefinitionEquipment.h"
#include "Interactable.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionDisabled, AActor*, DisabledInteractable);

// This class does not need to be modified.
UINTERFACE()
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTMIRROR_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnInteract(UInteractionComponent* InteractionComponent);
	UFUNCTION(BlueprintNativeEvent)
	FName GetInteractionPrompt();
	virtual FOnInteractionDisabled& GetOnInteractionDisabledDelegate() = 0;
	virtual UDefinitionItem* GetContainedItemDefinition() const { return nullptr; }
	UFUNCTION(BlueprintNativeEvent)
	EInteractionCategory GetInteractionCategory();
	UFUNCTION(BlueprintNativeEvent)
	USceneComponent* GetInteractionSnapTargetComponent();
	UFUNCTION(BlueprintNativeEvent)
	bool IsInterActionPossible(UInteractionComponent* InteractionComponent);
};

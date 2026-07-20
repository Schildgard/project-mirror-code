// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DefinitionItem.generated.h"


class AInteractablePickupObject;

UCLASS(Abstract, BlueprintType)
class PROJECTMIRROR_API UDefinitionItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FName ItemName = NAME_None;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> ItemTexture;

	UPROPERTY(EditAnywhere)
	FText ItemDescription;
	
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AInteractablePickupObject> WorldPickUpMesh;

public:
	FName GetItemName() const { return ItemName; }
	FText GetItemDescription() const { return ItemDescription; }
	TSoftClassPtr<AInteractablePickupObject> GetWorldPickUpActor() const { return WorldPickUpMesh; }
};

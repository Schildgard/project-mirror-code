// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectMirror/Interfaces/Interactable.h"
#include "InteractableObjectBase.generated.h"

UCLASS()
class PROJECTMIRROR_API AInteractableObjectBase : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AInteractableObjectBase();
	
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditAnywhere)
	FName EventID = NAME_None;
	
	UPROPERTY(EditAnywhere)
	USceneComponent* SnapTargetComponent = nullptr;
	
	UPROPERTY(EditAnywhere)
	EInteractionCategory InteractionCategory = EInteractionCategory::Interact;
	
	UPROPERTY(VisibleAnywhere)
	bool bIsInteractionPossible = true;
	UPROPERTY(EditAnywhere)
	bool bIsOneTimeInteraction = true;
	
	virtual void OnInteract_Implementation(UInteractionComponent* InteractingComponent) override;
	
public:
	UPROPERTY()
	FOnInteractionDisabled OnInteractionDisabled;
	virtual FOnInteractionDisabled& GetOnInteractionDisabledDelegate() override;
	virtual FName GetInteractionPrompt_Implementation() override;
	virtual bool IsInterActionPossible_Implementation(UInteractionComponent* InteractionComponent) override;
	virtual EInteractionCategory GetInteractionCategory_Implementation() override;
	virtual USceneComponent* GetInteractionSnapTargetComponent_Implementation() override;
	FName GetEventID()const{return EventID;}
};

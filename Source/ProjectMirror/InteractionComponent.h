// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Utility/EnumCollection.h"
#include "InteractionComponent.generated.h"

class UAnimInstanceSimple;
class USphereComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRadiusEnterDelegate, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRadiusExitDelegate, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentInteractableChanged, AActor*);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UInteractionComponent : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Components")
	USphereComponent* InteractionSphereComponent;

public:
	UInteractionComponent();

	FOnRadiusEnterDelegate OnRadiusEnter;
	FOnRadiusExitDelegate OnRadiusExit;
	FOnCurrentInteractableChanged OnCurrentInteractableChanged;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> InteractablesInRange;

	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractable;

	UPROPERTY()
	TObjectPtr<AActor> CachedInteractable;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void RegisterRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                         bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RegisterRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Interact();

	void AddInteractable(AActor* InteractableActor);
	UFUNCTION()
	void RemoveInteractable(AActor* InteractableActor);
	AActor* GetCurrentInteractable() const { return CurrentInteractable; }
	EInteractionCategory GetCurrentInteractionType() const;
	EInteractionHeight ResolveInteractionHeight() const;

	void SnapActorToCurrentInteractable();

	void CacheCurrentInteractable() { CachedInteractable = CurrentInteractable; }
	void EmptyInteractableCache() { CachedInteractable = nullptr; };
};

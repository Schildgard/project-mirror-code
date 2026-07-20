// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectStatic.h"
#include "InteractablePickupObject.generated.h"

class UEffectComponentBase;
class UDefinitionItem;

UCLASS()
class PROJECTMIRROR_API AInteractablePickupObject : public AInteractableObjectStatic
{
	GENERATED_BODY()

public:
	AInteractablePickupObject();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDefinitionItem> ContainedItemDefinition;

	UPROPERTY(EditAnywhere, Category="Equipment")
	TMap<TSubclassOf<UEffectComponentBase>, bool> InitialEffectStates;

	virtual void OnInteract_Implementation(UInteractionComponent* InteractingComponent) override;
	virtual UDefinitionItem* GetContainedItemDefinition() const override { return ContainedItemDefinition; }

	//Drop Logic
	FTimerHandle DropFallbackTimer;
	ECollisionEnabled::Type RestingCollision = ECollisionEnabled::QueryAndPhysics;

	virtual bool IsInterActionPossible_Implementation(UInteractionComponent* InteractionComponent) override;

	void DropFromPlayer();
	UFUNCTION()
	void OnDropRest(UPrimitiveComponent* PrimitiveComponent, FName BoneName);
	void FinishDrop();
	UPROPERTY()
	bool bIsFalling = false;

	bool IsDropBlocked();

	bool bIsStowProxy = false;

	AInteractablePickupObject* SpawnHandProxy(const UDefinitionItem* ItemDefinition);

public:
	void SetFalling() { bIsFalling = true; }
	void RemoveFromWorldObjectTracker() const;
	void AdjustObjectTransform();
	void SetEventID(const FName& NewEventID) { EventID = NewEventID; }
	EEquipmentSlot GetSlotToSpawnIn();

	//Blend Logic: slide the proxy mesh from the grab hand into the backup socket, then reveal the real weapon.
	void StartStowBlend(USkeletalMeshComponent* OwnerMesh, const FName& BackupSocket, UMeshComponent* WeaponMesh, float Duration);
	virtual void Destroyed() override;

protected:
	void FinishStowBlend();
	void RevealWeapon();

	bool bIsStowBlending = false;
	bool bWeaponRevealed = false;
	float StowBlendElapsed = 0.f;
	float StowBlendDuration = 0.f;
	FTransform StowBlendStartRelative;
	TWeakObjectPtr<UMeshComponent> WeaponMeshToReveal;
};

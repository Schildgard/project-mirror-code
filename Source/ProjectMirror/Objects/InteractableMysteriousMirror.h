// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectStatic.h"
#include "ProjectMirror/Interfaces/Focusable.h"
#include "InteractableMysteriousMirror.generated.h"

class UBoxComponent;
class USphereComponent;

UCLASS()
class PROJECTMIRROR_API AInteractableMysteriousMirror : public AInteractableObjectStatic, public IFocusable
{
	GENERATED_BODY()

public:
	AInteractableMysteriousMirror();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	USphereComponent* MirrorSpawnRadius;
	//@todo: repalce Despawnradius with sphere or box component for precision
	UPROPERTY(EditAnywhere)
	UBoxComponent* MirrorDespawnRadius;
	
	bool bIsPlayerFacingTowardsMirror;
	bool bIsStaircaseVisible;
	bool bIsPlayerInRegisterRadius;
	
	UPROPERTY()
	TObjectPtr<AActor> CachedPlayerActor;

	UFUNCTION()
	void RegisterRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RegisterRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:
	virtual void Tick(float DeltaTime) override;
	bool CheckPlayerFacingTowardsMirror();
	
	void ShowStaircase();
	void HideStaircase();
	
};

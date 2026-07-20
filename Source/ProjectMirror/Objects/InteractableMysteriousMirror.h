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
	TObjectPtr<USphereComponent> MirrorSpawnRadius;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> MirrorDespawnRadius;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MirrorPlaneComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneCaptureComponent2D> ReflectiveComponent;
	
	bool bIsPlayerFacingTowardsMirror;
	
	bool BIsMirroredObjectVisible;
	
	bool bIsPlayerInInnerRadius;
	
	bool bIsPlayerInOuterRadius;
	
	bool bIsPlayerInRange;
	
	const FName ActivateEventID = TEXT("OnMirrorFocus");
	const FName DeactivateEventID= TEXT("OnMirrorFocusLost");
	
	UPROPERTY()
	TObjectPtr<AActor> CachedPlayerActor;

	UFUNCTION()
	void RegisterInnerRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void RegisterOuterRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void RegisterInnerRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void RegisterOuterRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void UpdateReflection();
	
	
public:
	virtual void Tick(float DeltaTime) override;
	bool CheckPlayerFacingTowardsMirror();
	
	void TriggerActivateEvent();
	void TriggerDeactivateEvent();
	
};

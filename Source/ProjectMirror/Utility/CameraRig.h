// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraRig.generated.h"

UCLASS()
class PROJECTMIRROR_API ACameraRig : public AActor
{
	GENERATED_BODY()

public:
	ACameraRig();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	USpringArmComponent* CameraArm;
	
	UPROPERTY()
	UCameraComponent* Camera;
	
public:
	USpringArmComponent* GetCameraArm()const {return CameraArm;}
	UCameraComponent* GetCamera()const {return Camera;}
	
	void ApplyPose(const FVector& PivotOffset, const FRotator& ArmRotation, float ArmLength);
	
};

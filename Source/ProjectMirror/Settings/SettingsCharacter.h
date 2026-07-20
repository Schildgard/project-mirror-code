// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SettingsCharacter.generated.h"


UCLASS(config = ProjectMirror, DefaultConfig, meta = (DisplayName = "Character Settings"))
class PROJECTMIRROR_API USettingsCharacter : public UDeveloperSettings
{
	GENERATED_BODY()

	USettingsCharacter(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(Config, EditAnywhere)
	float InteractionRadius = 32.f;

	/*Camera*/
	UPROPERTY(Config, EditAnywhere, Category = "CameraSettings")
	float CameraDistance = 150.0f;

	UPROPERTY(Config, EditAnywhere, Category = "CameraSettings")
	FVector CameraOffset = FVector{0.f, 30.f, 30.f};

	UPROPERTY(Config, EditAnywhere, Category = "CameraSettings")
	bool CameraArmUsesPawnControlRotation = true;

	UPROPERTY(Config, EditAnywhere, Category = "CameraSettings")
	bool CameraUsesPawnControlRotation = false;

	/*Rotation*/
	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	bool bUseControllerRotationPitch = false;
	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	bool bUseControllerRotationYaw = false;
	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	bool bUseControllerRotationRoll = false;

	/*Movement*/
	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	float DefaultWalkingSpeed = 150.f;

	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	float DefaultRunningSpeed = 300.f;

	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	bool bOrientRotationToMovement = true;

	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	FRotator RotationRate = FRotator{0.f, 500.f, 0.f};

	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	float JumpZVelocity = 700.f;
	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	float AirControl = 0.35f;

	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	float MinAnalogWalkSpeed = 20.f;

	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	float BrakingDecelerationWalking = 2000.f;

	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	float BrakingDecelerationFalling = 1500.0f;

	//Interaction
	UPROPERTY(Config, EditAnywhere, Category = "CameraMovement")
	float InteractionHeightDistanceTolerance = 40.f;

	//Climbing
	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float AcceptableForwardDistance = 50.f;

	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float AcceptableUpwardDistance = 100.f;

	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float ClimbScanRadius = 10.f;

	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float MaxHeightToleranceBetweenSocketAndEdge = 60.f;

	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float MinHeightToleranceBetweenSocketAndEdge = 20.f;

	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float EdgeInwardOffset = 30.f;

	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float MinLedgeSurfaceUpDot = 0.7;

	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float HangBackDistance = 30.f;

	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float HangDownDistance = 60.f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Climbing")                                                                                                                                                                                                                                                                                                                                                                         
	TEnumAsByte<EObjectTypeQuery> ClimbableObjectType;
	
	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float SnapOnLedgeForwardOffset = 25.f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Climbing")
	float SnapOnLedgeUpwardOffset = 60.f;
	
	
};

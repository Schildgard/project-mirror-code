// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SettingsCamera.generated.h"


UCLASS(config = ProjectMirror, DefaultConfig, meta = (DisplayName = "Camera Settings"))
class PROJECTMIRROR_API USettingsCamera : public UDeveloperSettings
{
	GENERATED_BODY()
	
	USettingsCamera(const FObjectInitializer& ObjectInitializer);
public:
	
	UPROPERTY(Config, EditAnywhere, Category = "Sit Camera")
	FVector SitCameraPivotOffset = FVector::ZeroVector;
	
	UPROPERTY(Config, EditAnywhere, Category = "Sit Camera")
	FRotator SitCameraArmRotation = FRotator::ZeroRotator;

	UPROPERTY(Config, EditAnywhere, Category = "Sit Camera")
	float SitCameraArmLength = 250.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Sit Camera")
	float SitCameraBlendTime = 0.8f;
};

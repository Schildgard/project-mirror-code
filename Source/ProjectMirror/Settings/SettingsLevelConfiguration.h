// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SettingsLevelConfiguration.generated.h"

class UWidgetScreenFading;
/**
 * 
 */
UCLASS(config = ProjectMirror, DefaultConfig, meta = (DisplayName = "Level Settings"))
class PROJECTMIRROR_API USettingsLevelConfiguration : public UDeveloperSettings
{
	GENERATED_BODY()
	
	USettingsLevelConfiguration(const FObjectInitializer& ObjectInitializer);
public:
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UWorld> FirstLevel;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UWorld> MainMenuLevel;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UUserWidget> LoadingScreenWidgetClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UWidgetScreenFading> LevelTransitionFadingWidget;
	
	
	UPROPERTY(Config, EditAnywhere)
	float MinimumLoadingTime = 1.5f;
};

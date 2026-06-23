// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SettingsSaveGame.generated.h"

class UWidgetButtonSaveSlot;
/**
 * 
 */
UCLASS(config = ProjectMirror, DefaultConfig, meta = (DisplayName = "SaveGame Settings"))
class PROJECTMIRROR_API USettingsSaveGame : public UDeveloperSettings
{
	GENERATED_BODY()
	USettingsSaveGame(const FObjectInitializer& InitializerModule);
public:
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UWidgetButtonSaveSlot> SaveFileSlotClass;
};

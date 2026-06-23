// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SettingsUserInterface.generated.h"

class UCommonActivatableWidget;
class UWidgetButtonSlotContainerVertical;
class UCommonButtonStyle;
class UWidgetHUDLayout;
class UWidgetPauseMenu;
class UWidgetScreenTitleMenu;
class UWidgetTextDisplay;
class UWidgetButtonMenuSimple;
/**
 * 
 */
UCLASS(config= ProjectMirror, DefaultConfig, meta = (DisplayName = "UI Class Settings"))
class PROJECTMIRROR_API USettingsUserInterface : public UDeveloperSettings
{
	GENERATED_BODY()
	USettingsUserInterface(const FObjectInitializer& InitializerModule);
public:
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UCommonActivatableWidget> FallbackMenuScreenClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UWidgetHUDLayout> HUDLayoutClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UWidgetPauseMenu> InGameMenuClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UWidgetTextDisplay> InteractionPromptWidgetClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UWidgetButtonMenuSimple> DefaultMenuButtonClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UUserWidget> SavingThrobberClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UCommonButtonStyle> DefaultMenuButtonStyle;
	
	UPROPERTY(Config, EditAnywhere)
	TSubclassOf<UCommonButtonStyle> FocusMenuButtonStyle;
};

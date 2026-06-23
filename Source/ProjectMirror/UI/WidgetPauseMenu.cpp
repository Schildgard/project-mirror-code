// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetPauseMenu.h"
#include "Input/CommonUIInputTypes.h"
#include "HUDInGame.h"
#include "WidgetButtonMenuSimple.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ProjectMirror/SubsystemLevelLoading.h"

void UWidgetPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (USubsystemLevelLoading* LevelLoadingSystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
		if (ReturnToMainMenuButton)
		{
			ReturnToMainMenuButton->ButtonTextBlock->SetText(FText::FromString("Quit"));
			ReturnToMainMenuButton->OnClicked().AddUObject(LevelLoadingSystem, &USubsystemLevelLoading::ReturnToMainMenu);
		}
	}
	if (AHUDInGame* PlayerHUD = Cast<AHUDInGame>(GetOwningPlayer()->GetHUD()))
	{
		if (CloseInGameMenuButton)
		{
			CloseInGameMenuButton->ButtonTextBlock->SetText(FText::FromString("Close"));
			CloseInGameMenuButton->OnClicked().AddUObject(PlayerHUD, &AHUDInGame::HideInGameMenu);
		}
	}
}

void UWidgetPauseMenu::NativeOnActivated()
{
	Super::NativeOnActivated();
	if (InGameMenuInputData.IsNull())
	{
		return;
	}
	if (AHUDInGame* PlayerHUD = Cast<AHUDInGame>(GetOwningPlayer()->GetHUD()))
	{
	FBindUIActionArgs Args(InGameMenuInputData, FSimpleDelegate::CreateUObject(PlayerHUD, &AHUDInGame::HideInGameMenu));
	Args.bDisplayInActionBar = false;
	CloseInGameMenuHandle = RegisterUIActionBinding(Args);
	}
}

void UWidgetPauseMenu::NativeOnDeactivated()
{
	if (CloseInGameMenuHandle.IsValid())
	{
		CloseInGameMenuHandle.Unregister();
	}
	Super::NativeOnDeactivated();
}

TOptional<FUIInputConfig> UWidgetPauseMenu::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

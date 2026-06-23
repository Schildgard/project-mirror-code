// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetScreenTitleMenu.h"

#include "WidgetButtonMenuSimple.h"
#include "WidgetButtonSaveSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "ProjectMirror/SubsystemLevelLoading.h"
#include "ProjectMirror/SaveSystem/SubsystemSaveAndLoad.h"
#include "ProjectMirror/Settings/SettingsSaveGame.h"


static constexpr const TCHAR* StartGame = TEXT("Start New Game");
static constexpr const TCHAR* LoadGame = TEXT("Load Game");
static constexpr const TCHAR* QuitGame = TEXT("Quit");

TOptional<FUIInputConfig> UWidgetScreenTitleMenu::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

void UWidgetScreenTitleMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
		if (StartGameButton)
		{
			StartGameButton->OnClicked().AddUObject(LevelLoadSubsystem, &USubsystemLevelLoading::StartNewGame);
			StartGameButton->ButtonTextBlock->SetText(FText::FromString(StartGame));
		}
		if (QuitGameButton)
		{
			QuitGameButton->OnClicked().AddUObject(LevelLoadSubsystem, &USubsystemLevelLoading::QuitGame);
			QuitGameButton->ButtonTextBlock->SetText(FText::FromString(QuitGame));
		}
	}
	if (LoadGameButton)
	{
		LoadGameButton->OnClicked().AddUObject(this, &UWidgetScreenTitleMenu::ShowAllSaveFiles);
		LoadGameButton->ButtonTextBlock->SetText(FText::FromString(LoadGame));
	}
}

void UWidgetScreenTitleMenu::ShowAllSaveFiles()
{
	if (!IsValid(SaveSlotContainer))
	{
		return;
	}
	SaveSlotContainer->ClearChildren();

	if (const USettingsSaveGame* SaveGameSettings = GetDefault<USettingsSaveGame>())
	{
		if (!IsValid(SaveGameSettings->SaveFileSlotClass))
		{
			return;
		}

		if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
		{
			bool bIsFistSlot = true;
			for (const FSaveFileSlotData& SaveFile : SaveGameSubsystem->GetAllSaveFileData())
			{
				if (UWidgetButtonSaveSlot* NewSaveSlot = CreateWidget<UWidgetButtonSaveSlot>(
					this, SaveGameSettings->SaveFileSlotClass))
				{
					NewSaveSlot->LoadGameButton->ButtonTextBlock->SetText(FText::FromString(SaveFile.SaveFileID));
					NewSaveSlot->SetSaveGameReference(SaveFile.SaveFileID);
					SaveSlotContainer->AddChild(NewSaveSlot);
					if (bIsFistSlot)
					{
					NewSaveSlot->LoadGameButton->SetFocus();
					bIsFistSlot = false;
					}
				}
			}
		}
	}
}

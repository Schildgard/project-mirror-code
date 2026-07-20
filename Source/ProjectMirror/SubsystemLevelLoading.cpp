// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "SubsystemLevelLoading.h"

#include "GameWorld/SubsystemObjectEvents.h"
#include "Kismet/GameplayStatics.h"
#include "SaveSystem/SaveGameData.h"
#include "SaveSystem/SubsystemSaveAndLoad.h"
#include "Settings/SettingsLevelConfiguration.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"
#include "Utility/WidgetScreenFading.h"

void USubsystemLevelLoading::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnPreLoadMapDelegateHandle = FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &USubsystemLevelLoading::OnPreLoadMap);
}

void USubsystemLevelLoading::Deinitialize()
{
	Super::Deinitialize();
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
}

void USubsystemLevelLoading::OnPreLoadMap(const FString& MapName)
{
	ShowLoadingScreen();
}

void USubsystemLevelLoading::ShowLoadingScreen() const
{
	if (!GetMoviePlayer())
	{
		return;
	}
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;

	if (const USettingsLevelConfiguration* LevelConfigurationSettings = GetDefault<USettingsLevelConfiguration>())
	{
		LoadingScreen.MinimumLoadingScreenDisplayTime = LevelConfigurationSettings->MinimumLoadingTime;
		if (LevelConfigurationSettings->LoadingScreenWidgetClass)
		{
			UUserWidget* LoadingScreenWidget = CreateWidget<UUserWidget>(GetGameInstance(), LevelConfigurationSettings->LoadingScreenWidgetClass);
			if (!IsValid(LoadingScreenWidget))
			{
				return;
			}
			LoadingScreen.WidgetLoadingScreen = LoadingScreenWidget->TakeWidget();
		}
	}
	if (!LoadingScreen.WidgetLoadingScreen.IsValid())
	{
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
	}

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void USubsystemLevelLoading::OnFadeToBlackFinishedHandle()
{
	if (CachedFadingWidget.IsValid())
	{
		CachedFadingWidget->RemoveFromParent();
		CachedFadingWidget = nullptr;
	}
	if (CachedTargetLevelName.IsNone())
	{
		return;
	}
	UGameplayStatics::OpenLevel(this, CachedTargetLevelName);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USubsystemLevelLoading::OnPostChangedLevel);
}

void USubsystemLevelLoading::FadeFromScreenToBlack(const FName& LevelNameToCache)
{
	CachedTargetLevelName = LevelNameToCache;
	//@todo: Think about creating the widget by Player Controller and Add to PlayerScreen. This would be relevant for local multiplayer.


	if (const USettingsLevelConfiguration* LevelConfigurationSettings = GetDefault<USettingsLevelConfiguration>())
	{
		UWidgetScreenFading* ScreenFadingWidget = CreateWidget<UWidgetScreenFading>(GetGameInstance(), LevelConfigurationSettings->LevelTransitionFadingWidget);
		if (!IsValid(ScreenFadingWidget) || !ScreenFadingWidget->GetScreenFadingAnimation())
		{
			//If null widget, transmission without blend
			OnFadeToBlackFinishedHandle();
			return;
		}
		FWidgetAnimationDynamicEvent OnAnimFinishedDelegate;
		OnAnimFinishedDelegate.BindDynamic(this, &USubsystemLevelLoading::OnFadeToBlackFinishedHandle);
		
		CachedFadingWidget = ScreenFadingWidget;
		ScreenFadingWidget->AddToViewport();
		ScreenFadingWidget->FadeToBlack(OnAnimFinishedDelegate);
	}
}

void USubsystemLevelLoading::FadeFromBlackToScreen()
{
	if (const USettingsLevelConfiguration* LevelConfigurationSettings = GetDefault<USettingsLevelConfiguration>())
	{
		UWidgetScreenFading* ScreenFadingWidget = CreateWidget<UWidgetScreenFading>(GetGameInstance(), LevelConfigurationSettings->LevelTransitionFadingWidget);
		if (!IsValid(ScreenFadingWidget) || !ScreenFadingWidget->GetScreenFadingAnimation())
		{
			return;
		}
		
		FWidgetAnimationDynamicEvent OnAnimFinishedDelegate;
		OnAnimFinishedDelegate.BindDynamic(this, &USubsystemLevelLoading::OnFadeInFinishedHandle);
		CachedFadingWidget = ScreenFadingWidget;
		ScreenFadingWidget->AddToViewport();
		ScreenFadingWidget->FadeOutFromBlack(OnAnimFinishedDelegate);
	}
}

void USubsystemLevelLoading::OnFadeInFinishedHandle()
{
	if (CachedFadingWidget.IsValid())
	{
		CachedFadingWidget->RemoveFromParent();
		CachedFadingWidget = nullptr;
	}
}

void USubsystemLevelLoading::StartNewGame()
{
	if (const USettingsLevelConfiguration* LevelConfigurationSettings = GetDefault<USettingsLevelConfiguration>())
	{
		TSoftObjectPtr<UWorld> Level = LevelConfigurationSettings->FirstLevel;
		if (Level.IsNull())
		{
			return;
		}
		if (USubsystemObjectEvents* SubsystemObjectEvents = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
		{
			SubsystemObjectEvents->ResetWorldState();
		}
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, Level);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USubsystemLevelLoading::OnPostNewGameLevelLoad);
	}
}

void USubsystemLevelLoading::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}

void USubsystemLevelLoading::EnterNewLevel(const FName& LevelName)
{
	OnLevelTransitionStart.Broadcast(); // Command Player to Cache StatSettings

	if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		EventSubsystem->EmptyObjectEventListenerMap();
	}
	FadeFromScreenToBlack(LevelName);
}

void USubsystemLevelLoading::ReturnToMainMenu()
{
	if (const USettingsLevelConfiguration* LevelConfigurationSettings = GetDefault<USettingsLevelConfiguration>())
	{
		TSoftObjectPtr<UWorld> Level = LevelConfigurationSettings->MainMenuLevel;
		if (Level.IsNull())
		{
			return;
		}
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, Level);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USubsystemLevelLoading::OnReturnToMainMenuFinished);
	}
}

void USubsystemLevelLoading::LoadLevelFromSaveGame(USaveGameData* SaveGame)
{
	if (!IsValid(SaveGame) || SaveGame->GetCurrentLevel().IsNone())
	{
		return;
	}
	CachedSaveGame = SaveGame;
	if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		EventSubsystem->EmptyObjectEventListenerMap();
		EventSubsystem->PreloadActivatedObjectsFromSaveGame(SaveGame);
	}
	UGameplayStatics::OpenLevel(this, SaveGame->GetCurrentLevel());
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USubsystemLevelLoading::OnPostLevelLoad);
}

void USubsystemLevelLoading::OnPostLevelLoad(UWorld* LoadedLevel)
{
	CachedTargetLevelName = NAME_None;
	FadeFromBlackToScreen();
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	if (!IsValid(CachedSaveGame))
	{
		return;
	}
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		SaveGameSubsystem->CommandLoadLevelData.Broadcast(CachedSaveGame);
		OnLevelLoadedFromSaveGame.Broadcast(CachedSaveGame); //Player Gets his saved stats here
		CachedSaveGame = nullptr;
	}
}

void USubsystemLevelLoading::OnPostNewGameLevelLoad(UWorld* LoadedLevel)
{
	FadeFromBlackToScreen();
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		SaveGameSubsystem->SaveAllData();
	}
}

void USubsystemLevelLoading::OnPostChangedLevel(UWorld* ChangedLevel)
{
	CachedTargetLevelName = NAME_None;
	FadeFromBlackToScreen();
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		OnLevelTransitionEnd.Broadcast(); //Player gets StatsData back from Cache
		SaveGameSubsystem->LoadLevelData();
		SaveGameSubsystem->SaveAllData(); //Saves new Current Level and New Position of Player in Current Level to the SaveGame
	}
}

void USubsystemLevelLoading::CachePlayerLevelPersistentData(const FPlayerDataDiskAndMemory& PlayerData)
{
	CachedPlayerData = PlayerData;
}

void USubsystemLevelLoading::OnReturnToMainMenuFinished(UWorld* CurrentWorldContext)
{
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		SaveGameSubsystem->ResetCurrentSaveGameID();
	}

	if (USubsystemObjectEvents* SubsystemObjectEvents = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		SubsystemObjectEvents->ResetWorldState();
	}
}

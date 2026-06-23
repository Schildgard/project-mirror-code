// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "GameModeMainMenu.h"

#include "GameWorld/SubsystemBgm.h"
#include "Settings/SettingsSound.h"

AGameModeMainMenu::AGameModeMainMenu(const FObjectInitializer& ObjectInitializer)
{
	DefaultPawnClass = nullptr;
}

void AGameModeMainMenu::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		PlayerController->SetShowMouseCursor(true);
		PlayerController->SetInputMode(FInputModeUIOnly());
	}

	StartMainMenuBgm();
}

void AGameModeMainMenu::StartMainMenuBgm()
{
	if (USubsystemBgm* SubsystemBgm = GetGameInstance()->GetSubsystem<USubsystemBgm>())
	{
		if (const USettingsSound* Settings = GetDefault<USettingsSound>())
		{
			const FSoftObjectPath LoadedPath(GetWorld());
			const FSoftObjectPath NormalizedPath(UWorld::RemovePIEPrefix(LoadedPath.ToString()));

			const TSoftObjectPtr<USoundBase>* SoundPtr = Settings->LevelMusicMap.Find(TSoftObjectPtr<UWorld>(NormalizedPath));

			if (SoundPtr && !SoundPtr->IsNull())
			{
				SubsystemBgm->LoadSoundAsyncFromSoftPtr(*SoundPtr, ESoundCategory::BackgroundMusic, false);
			}
		}
	}
}
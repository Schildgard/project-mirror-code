// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "SubsystemSaveAndLoad.h"
#include "SaveGameData.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectMirror/SubsystemLevelLoading.h"
#include "ProjectMirror/Settings/SettingsUserInterface.h"

FString USubsystemSaveAndLoad::CreateNewSaveGameID()
{
	FString NewSaveGameID;
	for (int i = 0; i < 100; i++)
	{
		NewSaveGameID = FString::Printf(TEXT("ProjectMirror_SaveGame_%02i"), i);
		if (!UGameplayStatics::DoesSaveGameExist(NewSaveGameID, 0))
		{
			return NewSaveGameID;
		}
	}
	return NewSaveGameID;
}

void USubsystemSaveAndLoad::SaveAllData()
{
	if (bIsSaving)
	{
		return;
	}
	if (CurrentSaveGameID.IsEmpty())
	{
		CurrentSaveGameID = CreateNewSaveGameID();
	}

	USaveGameData* CurrentSaveGame = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveGameID, 0));
	if (!IsValid(CurrentSaveGame))
	{
		CurrentSaveGame = Cast<USaveGameData>(UGameplayStatics::CreateSaveGameObject(USaveGameData::StaticClass()));
		if (!IsValid(CurrentSaveGame))
		{
			return;
		}
	}
	bIsSaving = true;
	CurrentSaveGame->SlotID = CurrentSaveGameID;
	CommandSaveGame.Broadcast(CurrentSaveGame);
	CurrentSaveGame->SaveCurrentLevel(FName{UGameplayStatics::GetCurrentLevelName(this)});

	ShowSavingThrobber();
	FAsyncSaveGameToSlotDelegate SavedDelegate;
	SavedDelegate.BindUObject(this, &USubsystemSaveAndLoad::OnAsyncSaveFinished);
	UGameplayStatics::AsyncSaveGameToSlot(CurrentSaveGame, CurrentSaveGameID, 0, SavedDelegate);
}

void USubsystemSaveAndLoad::LoadLevelData()
{
	if (CurrentSaveGameID.IsEmpty())
	{
		return;
	}

	if (UGameplayStatics::DoesSaveGameExist(CurrentSaveGameID, 0))
	{
		USaveGameData* CurrentSaveGame = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(CurrentSaveGameID, 0));
		if (!IsValid(CurrentSaveGame))
		{
			return;
		}
		CommandLoadLevelData.Broadcast(CurrentSaveGame);
	}
}

void USubsystemSaveAndLoad::LoadSlotByID(const FString& SlotID)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotID, 0))
	{
		USaveGameData* CurrentSaveGame = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(SlotID, 0));
		if (!IsValid(CurrentSaveGame))
		{
			return;
		}
		CurrentSaveGameID = CurrentSaveGame->SlotID;

		if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
		{
			LevelLoadSubsystem->LoadLevelFromSaveGame(CurrentSaveGame);
		}
	}
}

void USubsystemSaveAndLoad::DeleteSlotByID(const FString& SlotID)
{
	UGameplayStatics::DeleteGameInSlot(SlotID, 0);
}

void USubsystemSaveAndLoad::ResetCurrentSaveGameID()
{
	CurrentSaveGameID.Empty();
}

FSaveFileSlotData USubsystemSaveAndLoad::ConvertSaveGameToSlotData(const USaveGameData* SaveGame)
{
	FSaveFileSlotData NewSaveGameData;
	NewSaveGameData.SaveFileID = SaveGame->SlotID;
	return NewSaveGameData;
}

TArray<FSaveFileSlotData> USubsystemSaveAndLoad::GetAllSaveFileData()
{
	TArray<FSaveFileSlotData> SaveFileData;
	SaveFileData.Reserve(100);

	for (int i = 0; i < 100; i++)
	{
		const FString NewSaveGameID = FString::Printf(TEXT("ProjectMirror_SaveGame_%02i"), i);
		if (UGameplayStatics::DoesSaveGameExist(NewSaveGameID, 0))
		{
			USaveGameData* SaveGame = Cast<USaveGameData>(UGameplayStatics::LoadGameFromSlot(NewSaveGameID, 0));
			if (IsValid(SaveGame))
			{
				SaveFileData.Add(ConvertSaveGameToSlotData(SaveGame));
			}
		}
	}
	return SaveFileData;
}

void USubsystemSaveAndLoad::ShowSavingThrobber()
{
	if (const USettingsUserInterface* UISettings = GetDefault<USettingsUserInterface>())
	{
		TSubclassOf<UUserWidget> ThrobberClass = UISettings->SavingThrobberClass;
		if (!IsValid(ThrobberClass))
		{
			return;
		}
		UUserWidget* ThrobberWidget = CreateWidget<UUserWidget>(GetGameInstance(), ThrobberClass);
		if (!IsValid(ThrobberWidget))
		{
			return;
		}
		CachedThrobber = ThrobberWidget;
		CachedThrobber->AddToViewport();
	}
}

void USubsystemSaveAndLoad::RemoveSavingThrobber()
{
	if (CachedThrobber.IsValid())
	{
		CachedThrobber->RemoveFromParent();
	}
	CachedThrobber.Reset();
}

void USubsystemSaveAndLoad::OnAsyncSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	bIsSaving = false;
	RemoveSavingThrobber();
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SubsystemSaveAndLoad.generated.h"

class USaveGameData;
DECLARE_MULTICAST_DELEGATE_OneParam(FCommandSaveGame, USaveGameData*);
DECLARE_MULTICAST_DELEGATE_OneParam(FCommandLoadGame, USaveGameData*);


USTRUCT(BlueprintType)
struct PROJECTMIRROR_API FSaveFileSlotData
{
	//Information which are displayed in a SaveFileSlotButton
	//@todo: Add Timestamp, Add LevelName, Add Screenshot?
	GENERATED_BODY()
	
	FString SaveFileID;
	
};

UCLASS()
class PROJECTMIRROR_API USubsystemSaveAndLoad : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	FString CurrentSaveGameID;
	
	UPROPERTY()
	TWeakObjectPtr<UUserWidget> CachedThrobber;
	bool bIsSaving = false;

public:
	
	FCommandSaveGame CommandSaveGame;
	FCommandLoadGame CommandLoadLevelData;
	
	FString CreateNewSaveGameID();
	
	void SaveAllData();
	void LoadLevelData();
	void LoadSlotByID(const FString& SlotID);
	void DeleteSlotByID(const FString& SlotID);
	void ResetCurrentSaveGameID();
	
	FSaveFileSlotData ConvertSaveGameToSlotData(const USaveGameData* SaveGame);
	
	TArray<FSaveFileSlotData> GetAllSaveFileData();
	
	FString GetCurrentSaveGameID()const {return CurrentSaveGameID;};
	void SetCurrentSaveGameID(const FString& NewSaveGameID){CurrentSaveGameID = NewSaveGameID;};
	void ShowSavingThrobber();
	void RemoveSavingThrobber();
	void OnAsyncSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);                                                                                                                                                                                                                                                                                                      
	
};

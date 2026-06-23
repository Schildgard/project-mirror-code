// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameData.generated.h"


USTRUCT()
struct FPlayerConditionData
{
	/*Contains Data that is hold through Level transitions. Mostly Player Stats..*/
	GENERATED_BODY()
	
	UPROPERTY()
	int32 CurrentHP= -1;
};

USTRUCT()
struct FPlayerTransformData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location = FVector::Zero();
	UPROPERTY()
	FRotator Rotation = FRotator::ZeroRotator;
};

USTRUCT()
struct FLevelSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	FString LevelName;
};

USTRUCT()
struct FWorldEventData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FName> ActivatedObjects;
};

UCLASS()
class PROJECTMIRROR_API USaveGameData : public USaveGame
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, FPlayerTransformData> PlayerSaveDataMap;
	
	UPROPERTY()
	TMap<FName, FPlayerConditionData> PlayerConditionDataMap;
	
	UPROPERTY()
	FWorldEventData WorldEventData;


public:
	UPROPERTY()
	FString SlotID;
	UPROPERTY()
	FName CurrentMapName;

	FPlayerTransformData* GetPlayerTransformData(const FName& ObjectID);
	void SavePlayerTransformData(const FName& ObjectID, const FPlayerTransformData& PlayerSaveData);
	FPlayerConditionData* GetPlayerConditionData(const FName& ObjectID);
	void SavePlayerConditionData(const FName& ObjectID, const FPlayerConditionData& PlayerConditionData);
	void SetWorldEventData(const FWorldEventData& WorldEventDataToSave) {WorldEventData = WorldEventDataToSave;};
	FWorldEventData* GetWorldEventData() {return &WorldEventData;};
	
	
	
	void SaveCurrentLevel(const FName& LevelName);
	

	FName GetCurrentLevel() const {return CurrentMapName;}

};

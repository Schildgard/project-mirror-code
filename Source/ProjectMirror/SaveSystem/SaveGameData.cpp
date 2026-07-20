// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "SaveGameData.h"

void USaveGameData::SavePlayerStaticData(const FName& ObjectID, const FPlayerDataDiskOnly& PlayerSaveData)
{
	if (ObjectID.IsNone())
	{
		return;
	}
	PlayerStaticSaveDataMap.Add(ObjectID, PlayerSaveData);
}

FPlayerDataDiskAndMemory* USaveGameData::GetPlayerConditionData(const FName& ObjectID)
{
	if (ObjectID.IsNone())
	{
		return nullptr;
	}
	return PlayerConditionDataMap.Find(ObjectID);
}

void USaveGameData::SavePlayerConditionData(const FName& ObjectID, const FPlayerDataDiskAndMemory& PlayerConditionData)
{
	if (ObjectID.IsNone())
	{
		return;
	}
	PlayerConditionDataMap.Add(ObjectID, PlayerConditionData);
}

void USaveGameData::SaveCurrentLevel(const FName& LevelName)
{
	CurrentMapName = LevelName;
}


FPlayerDataDiskOnly* USaveGameData::GetPlayerStaticData(const FName& ObjectID)
{
	if (ObjectID.IsNone())
	{
		return nullptr;
	}
	return PlayerStaticSaveDataMap.Find(ObjectID);
}

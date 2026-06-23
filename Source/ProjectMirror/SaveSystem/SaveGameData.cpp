// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "SaveGameData.h"

void USaveGameData::SavePlayerTransformData(const FName& ObjectID, const FPlayerTransformData& PlayerSaveData)
{
	if (ObjectID.IsNone())
	{
		return;
	}
	PlayerSaveDataMap.Add(ObjectID, PlayerSaveData);
}

FPlayerConditionData* USaveGameData::GetPlayerConditionData(const FName& ObjectID)
{
	if (ObjectID.IsNone())
	{
		return nullptr;
	}
	return PlayerConditionDataMap.Find(ObjectID);
}

void USaveGameData::SavePlayerConditionData(const FName& ObjectID, const FPlayerConditionData& PlayerConditionData)
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


FPlayerTransformData* USaveGameData::GetPlayerTransformData(const FName& ObjectID)
{
	if (ObjectID.IsNone())
	{
		return nullptr;
	}
	return PlayerSaveDataMap.Find(ObjectID);
}

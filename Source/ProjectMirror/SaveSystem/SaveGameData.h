// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ProjectMirror/Characters/Components/AttributeComponent.h"
#include "ProjectMirror/Data/DefinitionEquipment.h"
#include "SaveGameData.generated.h"


class UEffectComponentBase;
class UWeaponEffect;

USTRUCT()
struct FPlayerStatCondition
{
	
	GENERATED_BODY()
	
	UPROPERTY()
	float CurrentHealth = -1.0f;
};

USTRUCT()
struct FSavedEffectData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TSubclassOf<UEffectComponentBase> EffectClass;
	
	UPROPERTY()
	bool bIsActive = true;
	
};

USTRUCT()
struct FSavedEquipmentData
{
	GENERATED_BODY()

	UPROPERTY()
	FName EventID = NAME_None;

	UPROPERTY()
	TObjectPtr<UDefinitionEquipment> Definition = nullptr;

	UPROPERTY()
	TArray<FSavedEffectData> ActiveEffects;
};

USTRUCT()
struct FPlayerDataDiskAndMemory
{
	/*Contains Data that is hold through Level transitions. Mostly Player Stats like CurrentHP..*/
	GENERATED_BODY()
	
	UPROPERTY()
	FStatAttributes PlayerAttributes;

	UPROPERTY()
	FPlayerStatCondition CurrentStatCondition;

	UPROPERTY()
	TMap<EEquipmentSlot, FSavedEquipmentData> CurrentEquipment;
};

USTRUCT()
struct FPlayerDataDiskOnly
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

	UPROPERTY()
	TMap<FName, FTransform> DroppedItemMap;
	
	UPROPERTY()
	TMap<FName, float> ValueStateMap;
};

UCLASS()
class PROJECTMIRROR_API USaveGameData : public USaveGame
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, FPlayerDataDiskOnly> PlayerStaticSaveDataMap;

	UPROPERTY()
	TMap<FName, FPlayerDataDiskAndMemory> PlayerConditionDataMap;

	UPROPERTY()
	FWorldEventData WorldEventData;

public:
	UPROPERTY()
	FString SlotID;
	UPROPERTY()
	FName CurrentMapName;

	FPlayerDataDiskOnly* GetPlayerStaticData(const FName& ObjectID);
	void SavePlayerStaticData(const FName& ObjectID, const FPlayerDataDiskOnly& PlayerSaveData);

	FPlayerDataDiskAndMemory* GetPlayerConditionData(const FName& ObjectID);
	void SavePlayerConditionData(const FName& ObjectID, const FPlayerDataDiskAndMemory& PlayerConditionData);

	void SetWorldEventData(const FWorldEventData& WorldEventDataToSave) { WorldEventData = WorldEventDataToSave; };
	FWorldEventData* GetWorldEventData() { return &WorldEventData; };

	void SaveCurrentLevel(const FName& LevelName);

	FName GetCurrentLevel() const { return CurrentMapName; }
};

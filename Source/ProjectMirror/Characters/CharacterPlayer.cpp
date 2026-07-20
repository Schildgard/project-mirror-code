// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "CharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/EquipmentComponent.h"
#include "ProjectMirror/SubsystemLevelLoading.h"
#include "ProjectMirror/SaveSystem/SaveGameData.h"


ACharacterPlayer::ACharacterPlayer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	CharacterID = TEXT("Player");
}

void ACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
		LevelLoadSubsystem->OnLevelTransitionStart.AddUObject(this, &ACharacterPlayer::OnLevelTransitionStart);
		LevelLoadSubsystem->OnLevelLoadedFromSaveGame.AddUObject(this, &ACharacterPlayer::OnLevelLoadedFromSaveGame);
		LevelLoadSubsystem->OnLevelTransitionEnd.AddUObject(this, &ACharacterPlayer::OnLevelTransitionEnd);
	}
}

void ACharacterPlayer::OnCommandSaveData_Implementation(USaveGameData* SaveGameFile)
{
	if (!IsValid(SaveGameFile))
	{
		return;
	}
	Super::OnCommandSaveData_Implementation(SaveGameFile);

	FPlayerDataDiskOnly PlayerData;
	PlayerData.Location = GetActorLocation();
	PlayerData.Rotation = GetActorRotation();

	FPlayerDataDiskAndMemory CurrentPlayerConditionData;

	if (IsValid(EquipmentComponent))
	{
		CurrentPlayerConditionData.CurrentEquipment = EquipmentComponent->GetEquipmentForSave();
	}

	if (IsValid(AttributeComponent))
	{
		CurrentPlayerConditionData.PlayerAttributes = AttributeComponent->GetBaseAttributeValues();
		CurrentPlayerConditionData.CurrentStatCondition.CurrentHealth = AttributeComponent->GetCurrentHealth();
	}
	
	SaveGameFile->SavePlayerStaticData(CharacterID, PlayerData);
	SaveGameFile->SavePlayerConditionData(CharacterID, CurrentPlayerConditionData);
	
}

void ACharacterPlayer::OnCommandLoadData_Implementation(USaveGameData* SaveGameFile)
{
	if (!IsValid(SaveGameFile))
	{
		return;
	}
	Super::OnCommandLoadData_Implementation(SaveGameFile);

	if (const FPlayerDataDiskAndMemory* PlayerConditionDataPtr = SaveGameFile->GetPlayerConditionData(CharacterID))
	{
		SetStatsFromLevelPersistentData(*PlayerConditionDataPtr);
	}
}

void ACharacterPlayer::OnLevelTransitionStart()
{
	if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
		FPlayerDataDiskAndMemory CurrentPlayerLevelPersistentData;

		if (IsValid(EquipmentComponent))
		{
			CurrentPlayerLevelPersistentData.CurrentEquipment = EquipmentComponent->GetEquipmentForSave();
		}
		
		if (IsValid(AttributeComponent))
		{
			CurrentPlayerLevelPersistentData.PlayerAttributes = AttributeComponent->GetBaseAttributeValues();
			CurrentPlayerLevelPersistentData.CurrentStatCondition.CurrentHealth = AttributeComponent->GetCurrentHealth();
		}

		LevelLoadSubsystem->CachePlayerLevelPersistentData(CurrentPlayerLevelPersistentData);
	}
}

void ACharacterPlayer::OnLevelLoadedFromSaveGame(USaveGameData* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		return;
	}
	if (const FPlayerDataDiskOnly* PlayerTransformDataPtr = SaveGame->GetPlayerStaticData(CharacterID))
	{
		SetActorLocationAndRotation(PlayerTransformDataPtr->Location, PlayerTransformDataPtr->Rotation);
	}
	if (const FPlayerDataDiskAndMemory* PlayerConditionDataPtr = SaveGame->GetPlayerConditionData(CharacterID))
	{
		SetStatsFromLevelPersistentData(*PlayerConditionDataPtr);
	}
}


void ACharacterPlayer::OnLevelTransitionEnd()
{
	if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
		const FPlayerDataDiskAndMemory& CachedPlayerData = LevelLoadSubsystem->GetPlayerLevelPersistentData();
		SetStatsFromLevelPersistentData(CachedPlayerData);
	}
}

void ACharacterPlayer::SetStatsFromLevelPersistentData(const FPlayerDataDiskAndMemory& CharacterData)
{
	
	if (IsValid(AttributeComponent))
	{
		AttributeComponent->InitializeFromSave(CharacterData.PlayerAttributes, CharacterData.CurrentStatCondition.CurrentHealth);
	}

	if (IsValid(EquipmentComponent))
	{
		EquipmentComponent->SetEquipmentFromSave(CharacterData.CurrentEquipment);
	}
}
// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "CharacterPlayer.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectMirror/SubsystemLevelLoading.h"
#include "ProjectMirror/SaveSystem/SaveGameData.h"


ACharacterPlayer::ACharacterPlayer()
{
	// Set size for collision capsule// Copyright 2026 Leonard Kemenani. All Rights Reserved.
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

	FPlayerTransformData PlayerData;
	PlayerData.Location = GetActorLocation();
	PlayerData.Rotation = GetActorRotation();

	SaveGameFile->SavePlayerTransformData(CharacterID, PlayerData);
	SaveGameFile->SavePlayerConditionData(CharacterID,FPlayerConditionData{});
}

void ACharacterPlayer::OnCommandLoadData_Implementation(USaveGameData* SaveGameFile)
{
	if (!IsValid(SaveGameFile))
	{
		return;
	}
	Super::OnCommandLoadData_Implementation(SaveGameFile);
	
	if (const FPlayerConditionData* PlayerConditionDataPtr = SaveGameFile->GetPlayerConditionData(CharacterID))
	{
		SetStatsFromLevelPersistentData(* PlayerConditionDataPtr);
	}
}

void ACharacterPlayer::OnLevelTransitionStart()
{
	if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
		FPlayerConditionData CurrentPlayerLevelPersistentData;
		CurrentPlayerLevelPersistentData.CurrentHP = -1; //Get Values from Character Component
		
		LevelLoadSubsystem->CachePlayerLevelPersistentData(CurrentPlayerLevelPersistentData);
	}
}

void ACharacterPlayer::OnLevelLoadedFromSaveGame(USaveGameData* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		return;
	}
	if (const FPlayerTransformData* PlayerTransformDataPtr = SaveGame->GetPlayerTransformData(CharacterID))
	{
		SetActorLocationAndRotation(PlayerTransformDataPtr->Location, PlayerTransformDataPtr->Rotation);
	}
	if (const FPlayerConditionData* PlayerConditionDataPtr = SaveGame->GetPlayerConditionData(CharacterID))
	{
		SetStatsFromLevelPersistentData(*PlayerConditionDataPtr);
	}
}


void ACharacterPlayer::OnLevelTransitionEnd()
{
	if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
		const FPlayerConditionData& CachedPlayerData = LevelLoadSubsystem->GetPlayerLevelPersistentData();
		SetStatsFromLevelPersistentData(CachedPlayerData);
	}
}

void ACharacterPlayer::SetStatsFromLevelPersistentData(const FPlayerConditionData& CharacterData)
{
	//Set Character Stats
	ProvisoricTestStat = CharacterData.CurrentHP;
}

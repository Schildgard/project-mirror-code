// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "CharacterPlayer.generated.h"


struct FInputActionValue;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;

UCLASS()
class PROJECTMIRROR_API ACharacterPlayer : public ACharacterBase
{
	GENERATED_BODY()
	
protected:
	
	ACharacterPlayer();
	virtual void BeginPlay() override;
	
	int32 ProvisoricTestStat;
public:
	
	virtual void OnCommandSaveData_Implementation(USaveGameData* SaveGameFile) override;
	virtual void OnCommandLoadData_Implementation(USaveGameData* SaveGameFile) override;
	void OnLevelTransitionStart();
	void OnLevelTransitionEnd();
	void OnLevelLoadedFromSaveGame(USaveGameData* SaveGame);
	void SetStatsFromLevelPersistentData(const FPlayerConditionData& CharacterData);
};

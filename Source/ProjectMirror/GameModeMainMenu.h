// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API AGameModeMainMenu : public AGameModeBase
{
	GENERATED_BODY()
	
	
	AGameModeMainMenu(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	void StartMainMenuBgm();
};

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModePlayable.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API AGameModePlayable : public AGameModeBase
{
	GENERATED_BODY()
	
	AGameModePlayable(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay()override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
};

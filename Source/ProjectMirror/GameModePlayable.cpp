// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "GameModePlayable.h"

#include "EngineUtils.h"
#include "SubsystemLevelLoading.h"
#include "GameFramework/PlayerStart.h"
#include "UI/HUDInGame.h"

AGameModePlayable::AGameModePlayable(const FObjectInitializer& ObjectInitializer)
{
		HUDClass = AHUDInGame::StaticClass();
}

void AGameModePlayable::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
}

AActor* AGameModePlayable::ChoosePlayerStart_Implementation(AController* Player)
{
	if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
			const FName EntryPointID = LevelLoadSubsystem->GetPendingEntryPoint();
			LevelLoadSubsystem->SetPendingEntryPoint(NAME_None);
			for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
			{
				if ((*It)->PlayerStartTag == EntryPointID)
				{
					return *It;
				}
			}
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

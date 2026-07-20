// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveSystem/SaveGameData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SubsystemLevelLoading.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLevelTransitionStart);
DECLARE_MULTICAST_DELEGATE(FOnLevelTransitionEnd);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelLoadedFromSaveGame, USaveGameData*);

class USaveGameData;

UCLASS()
class PROJECTMIRROR_API USubsystemLevelLoading : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USaveGameData> CachedSaveGame;
	UPROPERTY()
	FPlayerDataDiskAndMemory CachedPlayerData;
	UPROPERTY()
	FName PendingEntryPoint = NAME_None;
	UPROPERTY()
	FName CachedTargetLevelName = NAME_None;
	TWeakObjectPtr<UUserWidget> CachedFadingWidget;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	//For Loading Screen
	void OnPreLoadMap(const FString &MapName);
	FDelegateHandle OnPreLoadMapDelegateHandle;
	void ShowLoadingScreen()const;
	
	UFUNCTION()
	void OnFadeToBlackFinishedHandle();
	UFUNCTION()
	void OnFadeInFinishedHandle();
	
	void FadeFromScreenToBlack(const FName& LevelNameToCache);
	void FadeFromBlackToScreen();
public:
	
	UFUNCTION()
	void StartNewGame();
	UFUNCTION()
	void QuitGame();
	UFUNCTION()
	void EnterNewLevel(const FName& LevelName);
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
	void LoadLevelFromSaveGame(USaveGameData* SaveGame);
	
	FOnLevelTransitionStart OnLevelTransitionStart;
	FOnLevelTransitionEnd OnLevelTransitionEnd;
	FOnLevelLoadedFromSaveGame OnLevelLoadedFromSaveGame;
	
	
	void OnPostLevelLoad(UWorld* LoadedLevel);
	void OnPostNewGameLevelLoad(UWorld* LoadedLevel);
	void OnPostChangedLevel(UWorld* ChangedLevel);
	void CachePlayerLevelPersistentData(const FPlayerDataDiskAndMemory& PlayerData);
	void OnReturnToMainMenuFinished(UWorld* CurrentWorldContext);
	const FPlayerDataDiskAndMemory& GetPlayerLevelPersistentData()const{return CachedPlayerData;};
	FName GetPendingEntryPoint() const{return PendingEntryPoint;};
	void SetPendingEntryPoint(const FName& NewPendingEntryPoint){PendingEntryPoint = NewPendingEntryPoint;};
};

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SubsystemBgm.generated.h"

UCLASS()
class PROJECTMIRROR_API USubsystemBgm : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<UAudioComponent>> MusicStack;

	UPROPERTY()
	TArray<TObjectPtr<UAudioComponent>> AmbientStack;

	UPROPERTY()
	TArray<TObjectPtr<UAudioComponent>> FadingOutAudioComponents;

	float FadeDuration = 3.f;

	void OnPreLoadLevel(const FString& LevelName);
	void OnPostLevelLoad(UWorld* LoadedLevel);

	void OnPostSoundLoaded(TSoftObjectPtr<USoundBase> LoadedSound, ESoundCategory Category, bool bPausePreviousSound, TWeakObjectPtr<UWorld> RequestedWorld);

	void PauseMusicAfterFadeOut(TWeakObjectPtr<UAudioComponent> WeakAudioToPause, ESoundCategory Category);
	void FadeOutAndDestroy(UAudioComponent* FinishedAudio);

	void FadeMusicIn(UAudioComponent* TargetAudioComponent, bool bWasSoundPaused);
	void OnMusicFadeOutFinished(UAudioComponent* FinishedAudio);

	UAudioComponent* CreateAndSetupAudioComponent(USoundBase* NewSound) const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void CleanUpStacksAndComponentsForLevelTransition(TArray<TObjectPtr<UAudioComponent>>& AudioStack);

public:
	void ResumeMusicAfterFadeOut(const ESoundCategory SoundCategory);
	TArray<TObjectPtr<UAudioComponent>>& GetAudioStack(const ESoundCategory Category);
	void LoadSoundAsyncFromSoftPtr(TSoftObjectPtr<USoundBase> LoadedSound, ESoundCategory Category, bool bPausePreviousSound,
	                               UAudioComponent* CachedAudioCompFromExtern = nullptr);
	void FadeMusicOut(UAudioComponent* TargetAudioComponent, bool bPauseSound, ESoundCategory SoundCategory);
	UAudioComponent* FindStackComponentBySound(const TArray<TObjectPtr<UAudioComponent>>& Stack, const USoundBase* Sound) const;

	//Bgm Areas
	void EnterBgmArea(const TObjectPtr<USoundBase> MusicToPlay, ESoundCategory Category);
	void ExitBgmArea(const TObjectPtr<USoundBase> MusicToStop, ESoundCategory Category);

	void HandleInGameMenuOpened();
	void HandleInGameMenuClosed();
};

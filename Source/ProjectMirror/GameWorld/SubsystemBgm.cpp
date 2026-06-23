// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "SubsystemBgm.h"
#include "Components/AudioComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectMirror/Settings/SettingsSound.h"


TArray<TObjectPtr<UAudioComponent>>& USubsystemBgm::GetAudioStack(const ESoundCategory Category)
{
	switch (Category)
	{
	case ESoundCategory::BackgroundMusic:
		return MusicStack;
	case ESoundCategory::AmbientSound:
		return AmbientStack;
	default: return MusicStack;
	}
}

void USubsystemBgm::ResumeMusicAfterFadeOut(const ESoundCategory SoundCategory)
{
	TArray<TObjectPtr<UAudioComponent>>& TargetAudioStack = GetAudioStack(SoundCategory);

	if (TargetAudioStack.IsEmpty())
	{
		return;
	}
	UAudioComponent* PausedAudio = TargetAudioStack.Top();
	if (!IsValid(PausedAudio))
	{
		return;
	}
	FadeMusicIn(PausedAudio, true);
}

void USubsystemBgm::FadeMusicOut(UAudioComponent* TargetAudioComponent, bool bPauseSound, ESoundCategory SoundCategory)
{
	if (!IsValid(TargetAudioComponent))
	{
		return;
	}

	if (bPauseSound)
	{
		FadingOutAudioComponents.Push(TargetAudioComponent);
		TargetAudioComponent->AdjustVolume(FadeDuration, 0.01f);
		//@todo: timer fires even if is not needed anymore. Fix this by making the timer a member.
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		FTimerHandle TimerHandle;
		TWeakObjectPtr<UAudioComponent> WeakTargetAudioComponent{TargetAudioComponent};
		const FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &USubsystemBgm::PauseMusicAfterFadeOut, WeakTargetAudioComponent, SoundCategory);

		TimerManager.SetTimer(TimerHandle, TimerDelegate, FadeDuration, false);
		return;
	}

	TArray<TObjectPtr<UAudioComponent>>& AudioStack = GetAudioStack(SoundCategory);
	AudioStack.Remove(TargetAudioComponent);
	FadeOutAndDestroy(TargetAudioComponent);
	ResumeMusicAfterFadeOut(SoundCategory);
}

void USubsystemBgm::FadeMusicIn(UAudioComponent* TargetAudioComponent, bool bWasSoundPaused)
{
	if (!IsValid(TargetAudioComponent))
	{
		return;
	}

	if (bWasSoundPaused)
	{
		TargetAudioComponent->SetPaused(false);
		TargetAudioComponent->AdjustVolume(FadeDuration, 1.0f);
		return;
	}

	TargetAudioComponent->FadeIn(FadeDuration, 1.0f);
}


void USubsystemBgm::OnPreLoadLevel(const FString& LevelName)
{
	CleanUpStacksAndComponentsForLevelTransition(MusicStack);
	CleanUpStacksAndComponentsForLevelTransition(AmbientStack);
}

void USubsystemBgm::OnPostLevelLoad(UWorld* LoadedLevel)
{
	if (const USettingsSound* SoundSettings = GetDefault<USettingsSound>())
	{
		const FSoftObjectPath LoadedPath(LoadedLevel);                                                                                                                                                                                   
		const FSoftObjectPath NormalizedPath(UWorld::RemovePIEPrefix(LoadedPath.ToString())); 
		
		const TSoftObjectPtr<USoundBase>* NewBgmSoftPtrRef = nullptr;
		const TSoftObjectPtr<USoundBase>* NewAmbientSoftPtrRef = nullptr;

		NewBgmSoftPtrRef = SoundSettings->LevelMusicMap.Find(TSoftObjectPtr<UWorld>(NormalizedPath));
		if (NewBgmSoftPtrRef && !NewBgmSoftPtrRef->IsNull())
		{
			LoadSoundAsyncFromSoftPtr(*NewBgmSoftPtrRef, ESoundCategory::BackgroundMusic, false);
		}
		NewAmbientSoftPtrRef = SoundSettings->LevelAmbientMap.Find(TSoftObjectPtr<UWorld>(NormalizedPath));
		if (NewAmbientSoftPtrRef && !NewAmbientSoftPtrRef->IsNull())
		{
			LoadSoundAsyncFromSoftPtr(*NewAmbientSoftPtrRef, ESoundCategory::AmbientSound, false);
		}
	}
}

void USubsystemBgm::OnPostSoundLoaded(TSoftObjectPtr<USoundBase> LoadedSound, ESoundCategory Category, bool bPausePreviousSound,
                                      TWeakObjectPtr<UWorld> RequestedWorld)
{
	if (!RequestedWorld.IsValid() || RequestedWorld != GetWorld())
	{
		return;
	}

	USoundBase* NewSound = LoadedSound.Get();
	if (!IsValid(NewSound) || Category == ESoundCategory::Undefined)
	{
		return;
	}
	TArray<TObjectPtr<UAudioComponent>>& TargetAudioStack = GetAudioStack(Category);
	UAudioComponent* NewAudio = CreateAndSetupAudioComponent(NewSound);
	if (!IsValid(NewAudio))
	{
		return;
	}

	FadeMusicIn(NewAudio, false);
	if (!TargetAudioStack.IsEmpty())
	{
		UAudioComponent* RecentAudio = TargetAudioStack.Top();
		if (IsValid(RecentAudio))
		{
			FadeMusicOut(RecentAudio, bPausePreviousSound, Category);
		}
	}

	TargetAudioStack.Push(NewAudio);
}

void USubsystemBgm::PauseMusicAfterFadeOut(TWeakObjectPtr<UAudioComponent> WeakAudioToPause, ESoundCategory Category)
{
	UAudioComponent* AudioToPause = WeakAudioToPause.Get();
	if (!IsValid(AudioToPause))
	{
		return;
	}
	FadingOutAudioComponents.Remove(AudioToPause);

	if ((!MusicStack.IsEmpty() && MusicStack.Top() == AudioToPause) || (!AmbientStack.IsEmpty() && AmbientStack.Top() == AudioToPause))
	{
		return;
	}

	AudioToPause->OnAudioFinishedNative.RemoveAll(this);
	AudioToPause->SetPaused(true);
}

void USubsystemBgm::FadeOutAndDestroy(UAudioComponent* FinishedAudio)
{
	if (!IsValid(FinishedAudio))
	{
		return;
	}

	FadingOutAudioComponents.Push(FinishedAudio);
	FinishedAudio->FadeOut(FadeDuration, 0.0f);
	FinishedAudio->OnAudioFinishedNative.AddUObject(this, &USubsystemBgm::OnMusicFadeOutFinished);
}

void USubsystemBgm::OnMusicFadeOutFinished(UAudioComponent* FinishedAudio)
{
	if (!IsValid(FinishedAudio))
	{
		return;
	}
	FinishedAudio->OnAudioFinishedNative.RemoveAll(this);
	FadingOutAudioComponents.Remove(FinishedAudio);
	FinishedAudio->DestroyComponent();
}

void USubsystemBgm::LoadSoundAsyncFromSoftPtr(TSoftObjectPtr<USoundBase> LoadedSound, ESoundCategory Category, bool bPausePreviousSound)
{
	TWeakObjectPtr<UWorld> RequestWorld = GetWorld();

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	FStreamableDelegate OnBgmLoadedDelegate = FStreamableDelegate::CreateUObject(this, &USubsystemBgm::OnPostSoundLoaded, LoadedSound,
	                                                                             Category, bPausePreviousSound, RequestWorld);
	StreamableManager.RequestAsyncLoad(LoadedSound.ToSoftObjectPath(), OnBgmLoadedDelegate);
}

UAudioComponent* USubsystemBgm::CreateAndSetupAudioComponent(USoundBase* NewSound) const
{
	if (!IsValid(NewSound))
	{
		return nullptr;
	}
	UAudioComponent* NewAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), NewSound, 1.0f, 1.0f, 0.f, nullptr, true, false);
	return NewAudioComponent;
}

void USubsystemBgm::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USubsystemBgm::OnPostLevelLoad);
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &USubsystemBgm::OnPreLoadLevel);
	
	if (const USettingsSound* SoundSettings = GetDefault<USettingsSound>())
	{
		FadeDuration = SoundSettings->SoundFadeDuration;
	}
}

void USubsystemBgm::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	Super::Deinitialize();
}

void USubsystemBgm::CleanUpStacksAndComponentsForLevelTransition(TArray<TObjectPtr<UAudioComponent>>& AudioStack)
{
	if (AudioStack.IsEmpty())
	{
		return;
	}
	UAudioComponent* CurrentPlayingAudio = AudioStack.Pop();
	for (UAudioComponent* PausedAudio : AudioStack)
	{
		if (!IsValid(PausedAudio))
		{
			continue;
		}
		PausedAudio->OnAudioFinishedNative.RemoveAll(this);
		FadingOutAudioComponents.Remove(PausedAudio);
		PausedAudio->DestroyComponent();
	}
	AudioStack.Reset();
	FadeOutAndDestroy(CurrentPlayingAudio);
}

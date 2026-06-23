// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "HUDInGame.h"

#include "WidgetPlayerHUD.h"
#include "WidgetHUDLayout.h"
#include "WidgetPauseMenu.h"
#include "WidgetTextDisplay.h"
#include "Blueprint/UserWidget.h"
#include "ProjectMirror/GameWorld/SubsystemBgm.h"
#include "ProjectMirror/Interfaces/Interactable.h"
#include "ProjectMirror/Settings/SettingsSound.h"
#include "ProjectMirror/Settings/SettingsUserInterface.h"


void AHUDInGame::BeginPlay()
{
	Super::BeginPlay();

	if (const USettingsUserInterface* UIClassSettings = GetDefault<USettingsUserInterface>())
	{
		HUDLayout = CreateWidget<UWidgetHUDLayout>(GetOwningPlayerController(), UIClassSettings->HUDLayoutClass);
		if (HUDLayout)
		{
			HUDLayout->AddToPlayerScreen();
			HUDLayout->GameHUDLayer->AddWidget<UWidgetPlayerHUD>(UWidgetPlayerHUD::StaticClass());
		}

		if (TSubclassOf<UWidgetTextDisplay> InteractionWidgetPromptClass = UIClassSettings->InteractionPromptWidgetClass)
		{
			InteractionPrompt = CreateWidget<UWidgetTextDisplay>(GetOwningPlayerController(), InteractionWidgetPromptClass);
		}
	}
}

void AHUDInGame::ShowInteractPrompt(const FName& PromptText)
{
	if (!IsValid(InteractionPrompt) || PromptText.IsNone())
	{
		return;
	}
	InteractionPrompt->SetWidgetText(PromptText);
	InteractionPrompt->AddToPlayerScreen();
}

void AHUDInGame::HideInteractPrompt()
{
	if (!IsValid(InteractionPrompt) || !InteractionPrompt->IsInViewport())
	{
		return;
	}
	InteractionPrompt->RemoveFromParent();
}

void AHUDInGame::ActivateMenuBgm() const
{
	if (USubsystemBgm* SubsystemBgm = GetGameInstance()->GetSubsystem<USubsystemBgm>())
	{
		if (const USettingsSound* Settings = GetDefault<USettingsSound>())
		{
			const TSoftObjectPtr<USoundBase> SoundSoftPtr = Settings->IngGameMenuMusic;
			if (SoundSoftPtr.IsNull())
			{
				return;
			}
			SubsystemBgm->LoadSoundAsyncFromSoftPtr(SoundSoftPtr, ESoundCategory::BackgroundMusic, true);
			//Fade out Ambient
			TArray<TObjectPtr<UAudioComponent>>& AmbientStack = SubsystemBgm->GetAudioStack(ESoundCategory::AmbientSound);
			if (AmbientStack.IsEmpty())
			{
				return;
			}
			if (UAudioComponent* CurrentAmbient = AmbientStack.Top())
			{
				SubsystemBgm->FadeMusicOut(CurrentAmbient, true, ESoundCategory::AmbientSound);
			}
		}
	}
}

void AHUDInGame::EndMenuBgm() const
{
	if (USubsystemBgm* SubsystemBgm = GetGameInstance()->GetSubsystem<USubsystemBgm>())
	{
		//Fade out CurrentMusic
		TArray<TObjectPtr<UAudioComponent>>& MusicStack = SubsystemBgm->GetAudioStack(ESoundCategory::BackgroundMusic);
		if (MusicStack.IsEmpty())
		{
			return;
		}
		if (UAudioComponent* CurrentMusic = MusicStack.Top())
		{
			//Resume of old bgm is handled in FadeMusicOutFunction
			SubsystemBgm->FadeMusicOut(CurrentMusic, false, ESoundCategory::BackgroundMusic);
		}
		//Fade Ambient back in
		SubsystemBgm->ResumeMusicAfterFadeOut(ESoundCategory::AmbientSound);
	}
}

void AHUDInGame::PushInGameMenu()
{
	if (!HUDLayout || !HUDLayout->GameHUDLayer)
	{
		return;
	}
	if (const USettingsUserInterface* UIClassSettings = GetDefault<USettingsUserInterface>())
	{
		HUDLayout->GameHUDLayer->AddWidget<UWidgetPauseMenu>(UIClassSettings->InGameMenuClass);
		ActivateMenuBgm();
	}
}

void AHUDInGame::HideInGameMenu()
{
	if (!HUDLayout || !HUDLayout->GameHUDLayer)
	{
		return;
	}

	if (UCommonActivatableWidget* ActiveWidget = HUDLayout->GameHUDLayer->GetActiveWidget())
	{
		ActiveWidget->DeactivateWidget();
		EndMenuBgm();
	}
}

void AHUDInGame::OnInteractableChanged(AActor* CurrentInteractable)
{
	if (!IsValid(CurrentInteractable))
	{
		HideInteractPrompt();
		return;
	}
	
	const FName& PromptText = IInteractable::Execute_GetInteractionPrompt(CurrentInteractable);
	ShowInteractPrompt(PromptText); //Add Logic that varies the text on the Interactable type..
}

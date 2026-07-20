// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "HUDInGame.h"

#include "VMProgressbar.h"
#include "WidgetPlayerHUD.h"
#include "WidgetHUDLayout.h"
#include "WidgetPauseMenu.h"
#include "WidgetTextDisplay.h"
#include "Blueprint/UserWidget.h"
#include "ProjectMirror/Characters/Components/AttributeComponent.h"
#include "ProjectMirror/Characters/Components/StaminaComponent.h"
#include "ProjectMirror/GameWorld/SubsystemBgm.h"
#include "ProjectMirror/Interfaces/Interactable.h"
#include "ProjectMirror/Settings/SettingsUserInterface.h"


void AHUDInGame::BeginPlay()
{
	Super::BeginPlay();

	VMHealthBar = NewObject<UVMProgressbar>(this);
	VMStaminaBar = NewObject<UVMProgressbar>(this);
	if (const USettingsUserInterface* UIClassSettings = GetDefault<USettingsUserInterface>())
	{
		HUDLayout = CreateWidget<UWidgetHUDLayout>(GetOwningPlayerController(), UIClassSettings->HUDLayoutClass);
		if (HUDLayout)
		{
			HUDLayout->AddToPlayerScreen();
			if (UWidgetPlayerHUD* PlayerHUD = HUDLayout->GameHUDLayer->AddWidget<UWidgetPlayerHUD>(UIClassSettings->PlayerHUDClass))
			{
				PlayerHUD->SetViewModels(VMHealthBar, VMStaminaBar);
			}
		}

		if (TSubclassOf<UWidgetTextDisplay> InteractionWidgetPromptClass = UIClassSettings->InteractionPromptWidgetClass)
		{
			InteractionPrompt = CreateWidget<UWidgetTextDisplay>(GetOwningPlayerController(), InteractionWidgetPromptClass);
		}
	}

	if (USubsystemBgm* BgmSubsystem = GetGameInstance()->GetSubsystem<USubsystemBgm>())
	{
		OnInGameMenuOpened.AddUObject(BgmSubsystem, &USubsystemBgm::HandleInGameMenuOpened);
		OnInGameMenuClosed.AddUObject(BgmSubsystem, &USubsystemBgm::HandleInGameMenuClosed);
	}

	if (APlayerController* PlayerController = GetOwningPlayerController())
	{
		PlayerController->OnPossessedPawnChanged.AddDynamic(this, &AHUDInGame::HandlePossessedPawnChanged);
		BindPlayerComponents(PlayerController->GetPawn());
	}
}

void AHUDInGame::BeginDestroy()
{
	Super::BeginDestroy();

	if (UGameInstance* Instance = GetGameInstance())
	{
		if (USubsystemBgm* BgmSubsystem = Instance->GetSubsystem<USubsystemBgm>())
		{
			OnInGameMenuOpened.RemoveAll(BgmSubsystem);
			OnInGameMenuClosed.RemoveAll(BgmSubsystem);
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

void AHUDInGame::PushInGameMenu()
{
	if (!HUDLayout || !HUDLayout->GameHUDLayer)
	{
		return;
	}
	if (const USettingsUserInterface* UIClassSettings = GetDefault<USettingsUserInterface>())
	{
		HUDLayout->GameHUDLayer->AddWidget<UWidgetPauseMenu>(UIClassSettings->InGameMenuClass);
		OnInGameMenuOpened.Broadcast();
	}
}

void AHUDInGame::HideInGameMenu()
{
	if (!HUDLayout || !HUDLayout->GameHUDLayer)
	{
		return;
	}

	//Reset Input Settings aber Camera blend in finished. By Remove widget only as fallback
	if (UCommonActivatableWidget* ActiveWidget = HUDLayout->GameHUDLayer->GetActiveWidget())
	{
		ActiveWidget->DeactivateWidget();
		OnInGameMenuClosed.Broadcast();
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

void AHUDInGame::BindPlayerComponents(APawn* NewPawn)
{
	UnbindPlayerComponents();

	if (!IsValid(NewPawn))
	{
		return;
	}

	CachedAttributeComponent = NewPawn->FindComponentByClass<UAttributeComponent>();
	if (IsValid(CachedAttributeComponent))
	{
		CachedAttributeComponent->OnHealthChanged.AddUObject(this, &AHUDInGame::HandleHealthChanged);
		HandleHealthChanged();
	}

	CachedStaminaComponent = NewPawn->FindComponentByClass<UStaminaComponent>();
	if (IsValid(CachedStaminaComponent))
	{
		CachedStaminaComponent->OnStaminaChanged.AddUObject(this, &AHUDInGame::HandleStaminaChanged);
		HandleStaminaChanged();
	}
}

void AHUDInGame::UnbindPlayerComponents()
{
	if (IsValid(CachedAttributeComponent))
	{
		CachedAttributeComponent->OnHealthChanged.RemoveAll(this);
	}
	if (IsValid(CachedStaminaComponent))
	{
		CachedStaminaComponent->OnStaminaChanged.RemoveAll(this);
	}
	CachedAttributeComponent = nullptr;
	CachedStaminaComponent = nullptr;
}

void AHUDInGame::HandleHealthChanged()
{
	if (!IsValid(VMHealthBar) || !IsValid(CachedAttributeComponent))
	{
		return;
	}
	VMHealthBar->SetMaxValue(CachedAttributeComponent->GetMaxHealth());
	VMHealthBar->SetCurrentValue(CachedAttributeComponent->GetCurrentHealth());
}

void AHUDInGame::HandleStaminaChanged()
{
	if (!IsValid(VMStaminaBar) || !IsValid(CachedStaminaComponent))
	{
		return;
	}
	VMStaminaBar->SetMaxValue(CachedStaminaComponent->GetMaxStamina());
	VMStaminaBar->SetCurrentValue(CachedStaminaComponent->GetCurrentStamina());
}

void AHUDInGame::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	BindPlayerComponents(NewPawn);
}

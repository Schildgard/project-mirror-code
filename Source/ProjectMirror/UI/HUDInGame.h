// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDInGame.generated.h"

class UAttributeComponent;
class UStaminaComponent;
class UVMProgressbar;
class UWidgetHUDLayout;
class UWidgetTextDisplay;

DECLARE_MULTICAST_DELEGATE(FOnInGameMenuOpened);
DECLARE_MULTICAST_DELEGATE(FOnInGameMenuClosed);
UCLASS()
class PROJECTMIRROR_API AHUDInGame : public AHUD
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	UPROPERTY()
	TObjectPtr<UWidgetTextDisplay> InteractionPrompt;

	UPROPERTY()
	TObjectPtr<UWidgetHUDLayout> HUDLayout;

	UPROPERTY()
	TObjectPtr<UVMProgressbar> VMHealthBar;

	UPROPERTY()
	TObjectPtr<UVMProgressbar> VMStaminaBar;

	UPROPERTY()
	TObjectPtr<UAttributeComponent> CachedAttributeComponent;

	UPROPERTY()
	TObjectPtr<UStaminaComponent> CachedStaminaComponent;

	void ShowInteractPrompt(const FName& PromptText);
	void HideInteractPrompt();

public:
	UFUNCTION()
	void HideInGameMenu();
	void PushInGameMenu();
	void OnInteractableChanged(AActor* CurrentInteractable);

	void BindPlayerComponents(APawn* NewPawn);
	void UnbindPlayerComponents();
	void HandleHealthChanged();
	void HandleStaminaChanged();

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	FOnInGameMenuOpened OnInGameMenuOpened;
	FOnInGameMenuClosed OnInGameMenuClosed;
};

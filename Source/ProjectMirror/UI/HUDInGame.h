// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDInGame.generated.h"

class UWidgetHUDLayout;
class UWidgetTextDisplay;
/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API AHUDInGame : public AHUD
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<UWidgetTextDisplay> InteractionPrompt;
	
	UPROPERTY()
	TObjectPtr<UWidgetHUDLayout> HUDLayout;

	
	void ShowInteractPrompt(const FName& PromptText);
	void HideInteractPrompt();
	void ActivateMenuBgm() const;
	void EndMenuBgm() const;

public:
	UFUNCTION()
	void HideInGameMenu();
	void PushInGameMenu();
	void OnInteractableChanged(AActor* CurrentInteractable);
};

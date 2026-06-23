// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDOutGame.generated.h"

class UCommonActivatableWidget;
class UWidgetHUDLayout;
class UWidgetScreenTitleMenu;
/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API AHUDOutGame : public AHUD
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCommonActivatableWidget> MenuClass;

	virtual void BeginPlay() override;
	void AddTitleMenuWidgetToStack();
	
	UPROPERTY()
	TObjectPtr<UWidgetHUDLayout> HUDLayout;
};

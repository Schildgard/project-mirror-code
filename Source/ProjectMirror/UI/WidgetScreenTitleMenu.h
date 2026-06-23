// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "WidgetScreenTitleMenu.generated.h"


class UVerticalBox;
class UWidgetButtonMenuSimple;
/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UWidgetScreenTitleMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetButtonMenuSimple> StartGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetButtonMenuSimple> QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetButtonMenuSimple> LoadGameButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> SaveSlotContainer;
	
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;  
	
	

	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void ShowAllSaveFiles();
};

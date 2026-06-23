// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "WidgetPauseMenu.generated.h"

class UWidgetButtonMenuSimple;

UCLASS()
class PROJECTMIRROR_API UWidgetPauseMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetButtonMenuSimple> ReturnToMainMenuButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetButtonMenuSimple> CloseInGameMenuButton;
	
	UPROPERTY(EditAnywhere)
	FDataTableRowHandle InGameMenuInputData;
	
	FUIActionBindingHandle CloseInGameMenuHandle;

	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;  

};

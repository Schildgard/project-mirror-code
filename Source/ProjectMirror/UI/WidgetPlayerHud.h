// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "WidgetPlayerHUD.generated.h"

class UWidgetProgressbar;
class UVMProgressbar;


UCLASS()
class PROJECTMIRROR_API UWidgetPlayerHUD : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UWidgetProgressbar> HealthBar;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess))
	TObjectPtr<UWidgetProgressbar> StaminaBar;

public:
	void SetViewModels(UVMProgressbar* HealthBarVM, UVMProgressbar* StaminaBarVM);
};

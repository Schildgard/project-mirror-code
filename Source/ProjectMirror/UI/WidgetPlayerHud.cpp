// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetPlayerHUD.h"
#include "WidgetProgressbar.h"

TOptional<FUIInputConfig> UWidgetPlayerHUD::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently);
}

void UWidgetPlayerHUD::SetViewModels(UVMProgressbar* HealthBarVM, UVMProgressbar* StaminaBarVM)
{
	if (IsValid(HealthBar))
	{
		HealthBar->SetViewModel(HealthBarVM);
	}
	if (IsValid(StaminaBar))
	{
		StaminaBar->SetViewModel(StaminaBarVM);
	}
}

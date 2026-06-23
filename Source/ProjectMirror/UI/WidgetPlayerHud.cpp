// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetPlayerHUD.h"

TOptional<FUIInputConfig> UWidgetPlayerHUD::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently);
}

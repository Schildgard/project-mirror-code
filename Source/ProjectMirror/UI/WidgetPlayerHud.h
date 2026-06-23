// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "WidgetPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UWidgetPlayerHUD : public UCommonActivatableWidget
{
	GENERATED_BODY()
protected:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
};

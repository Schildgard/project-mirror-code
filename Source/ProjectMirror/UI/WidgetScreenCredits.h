// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "WidgetScreenCredits.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UWidgetScreenCredits : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> TextScrollingAnimation;
	
	FWidgetAnimationDynamicEvent OnCreditsAnimFinished;
	
	virtual void NativeConstruct() override;
	UFUNCTION()
	void ReturnToMainMenu();
};

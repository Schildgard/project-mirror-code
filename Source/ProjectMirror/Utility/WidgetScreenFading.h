// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetScreenFading.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UWidgetScreenFading : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* ScreenFadingAnimation;
	
public:
	UWidgetAnimation* GetScreenFadingAnimation() { return ScreenFadingAnimation; }
	void FadeToBlack(const FWidgetAnimationDynamicEvent& OnFadeToBlackFinished);
	void FadeOutFromBlack(const FWidgetAnimationDynamicEvent& OnFadeFromBlackFinished);
	
};

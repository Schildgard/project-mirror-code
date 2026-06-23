// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetScreenFading.h"

#include "Animation/WidgetAnimation.h"

void UWidgetScreenFading::FadeToBlack(const FWidgetAnimationDynamicEvent& OnFadeToBlackFinished)
{
	if (!ScreenFadingAnimation)
	{
		return;
	}
	BindToAnimationFinished(ScreenFadingAnimation, OnFadeToBlackFinished);
	PlayAnimation(ScreenFadingAnimation);
}

void UWidgetScreenFading::FadeOutFromBlack(const FWidgetAnimationDynamicEvent& OnFadeFromBlackFinished)
{
	if (!ScreenFadingAnimation)
	{
		return;
	}
	BindToAnimationFinished(ScreenFadingAnimation, OnFadeFromBlackFinished);
	PlayAnimationReverse(ScreenFadingAnimation);
}

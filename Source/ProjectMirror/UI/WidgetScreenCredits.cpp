// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetScreenCredits.h"

#include "Animation/WidgetAnimation.h"
#include "ProjectMirror/SubsystemLevelLoading.h"

void UWidgetScreenCredits::NativeConstruct()
{
	Super::NativeConstruct();
	if (!TextScrollingAnimation)
	{
		return;
	}
	OnCreditsAnimFinished.BindDynamic(this, &UWidgetScreenCredits::ReturnToMainMenu);
	BindToAnimationFinished(TextScrollingAnimation, OnCreditsAnimFinished);
	PlayAnimationForward(TextScrollingAnimation);
}


void UWidgetScreenCredits::ReturnToMainMenu()
{
	if (USubsystemLevelLoading* SubsystemLevelLoading = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
	{
		SubsystemLevelLoading->ReturnToMainMenu();
	}
}

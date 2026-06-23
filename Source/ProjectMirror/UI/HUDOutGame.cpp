// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "HUDOutGame.h"
#include "WidgetHUDLayout.h"
#include "WidgetScreenTitleMenu.h"
#include "ProjectMirror/Settings/SettingsUserInterface.h"

void AHUDOutGame::BeginPlay()
{
	Super::BeginPlay();
	AddTitleMenuWidgetToStack();
}

void AHUDOutGame::AddTitleMenuWidgetToStack()
{
	//@todo there probably no reason anymore for a Hud layout which only uses one stack anyway. Test in safe environment if the HUDLayout can be removed and replaced by a single stack, or at least remove the unused stack.
	if (const USettingsUserInterface* UIClassSettings = GetDefault<USettingsUserInterface>())
	{
		HUDLayout = CreateWidget<UWidgetHUDLayout>(GetOwningPlayerController(), UIClassSettings->HUDLayoutClass);
		if (!HUDLayout)
		{
			return;
		}
		HUDLayout->AddToPlayerScreen();
		if (MenuClass)
		{
			HUDLayout->GameHUDLayer->AddWidget<UCommonActivatableWidget>(MenuClass);
			return;
		}
		if (UIClassSettings->FallbackMenuScreenClass)
		{
			HUDLayout->GameHUDLayer->AddWidget<UCommonActivatableWidget>(UIClassSettings->FallbackMenuScreenClass);
		}
	}
}

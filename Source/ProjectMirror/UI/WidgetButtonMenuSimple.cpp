// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetButtonMenuSimple.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectMirror/Settings/SettingsUserInterface.h"

void UWidgetButtonMenuSimple::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);

	if (!IsValid(FocusStyle))
	{
		return;
	}

	if (!IsValid(FocusStyle))
	{
		if (const USettingsUserInterface* UISettings = GetDefault<USettingsUserInterface>())
		{
			SetStyle(UISettings->FocusMenuButtonStyle);
		}
		return;
	}
	SetStyle(FocusStyle);
}

void UWidgetButtonMenuSimple::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
	if (!IsValid(FocusLeftSound))
	{
		return;
	}
	UGameplayStatics::PlaySound2D(GetWorld(), FocusLeftSound, 0.5, 0.5f);

	if (!IsValid(DefaultStyle))
	{
		if (const USettingsUserInterface* UISettings = GetDefault<USettingsUserInterface>())
		{
			SetStyle(UISettings->DefaultMenuButtonStyle);
		}
		return;
	}
	SetStyle(DefaultStyle);
}

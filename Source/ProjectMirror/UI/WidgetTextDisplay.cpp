// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetTextDisplay.h"

#include "Components/TextBlock.h"

void UWidgetTextDisplay::SetWidgetText(const FName& PromptText)
{
	TextBlock->SetText(FText::FromName(PromptText));
}

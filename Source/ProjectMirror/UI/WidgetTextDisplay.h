// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetTextDisplay.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UWidgetTextDisplay : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock;
	
public:
	void SetWidgetText(const FName& PromptText);
};

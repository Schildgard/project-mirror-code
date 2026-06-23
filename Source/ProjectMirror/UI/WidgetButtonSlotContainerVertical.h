// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonUserWidget.h"
#include "WidgetButtonSlotContainerVertical.generated.h"

class UVerticalBox;

UCLASS()
class PROJECTMIRROR_API UWidgetButtonSlotContainerVertical : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBox;
	
	
};

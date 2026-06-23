// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMInteractWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UVMInteractWidget : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
	UPROPERTY(FieldNotify, Getter, Setter)
	FText WidgetText;
public:
	UFUNCTION(FieldNotify, BlueprintPure)
	FText GetWidgetText() const{return WidgetText;};
	
	void SetWidgetText(const FText& NewWidgetText)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(WidgetText, NewWidgetText))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(WidgetText);
		}
	}
	
};

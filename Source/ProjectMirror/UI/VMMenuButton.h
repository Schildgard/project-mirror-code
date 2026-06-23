// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMMenuButton.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UVMMenuButton : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
	UPROPERTY(FieldNotify, Setter, Getter)
	FText ButtonText;
	
	public:
	
	UFUNCTION()
	void SetButtonText(const FText& NewText)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(ButtonText, NewText))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(ButtonText);
		}
	}
	
	UFUNCTION(BlueprintPure, FieldNotify)
	FText GetButtonText() const{return  ButtonText;};
	
};

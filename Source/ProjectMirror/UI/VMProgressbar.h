// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMProgressbar.generated.h"


UCLASS()
class PROJECTMIRROR_API UVMProgressbar : public UMVVMViewModelBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess))
	float CurrentValue = 0.0f;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess))
	float MaxValue = 0.0f;

public:

	float GetCurrentValue() const { return CurrentValue; };
	
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetMaxValue() const { return MaxValue; };
	
	void SetCurrentValue(float NewValue)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(CurrentValue, NewValue))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetPercent);
		}
	}
	
	void SetMaxValue(float NewValue)
	{
		if (UE_MVVM_SET_PROPERTY_VALUE(MaxValue, NewValue))
		{
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetMaxValue);
			UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetPercent);
		}
	}
	
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetPercent() const
	{
		return MaxValue > 0.0f ? CurrentValue / MaxValue : 0.0f;
	}
};

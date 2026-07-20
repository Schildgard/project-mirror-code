// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetProgressbar.h"
#include "VMProgressbar.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"

void UWidgetProgressbar::NativeDestruct()
{
	if (IsValid(ViewModel))
	{
		if (PercentChangedHandle.IsValid())
		{
			ViewModel->RemoveFieldValueChangedDelegate(UVMProgressbar::FFieldNotificationClassDescriptor::GetPercent, PercentChangedHandle);
			PercentChangedHandle.Reset();
		}
		if (MaxChangedHandle.IsValid())
		{
			ViewModel->RemoveFieldValueChangedDelegate(UVMProgressbar::FFieldNotificationClassDescriptor::GetMaxValue, MaxChangedHandle);
			MaxChangedHandle.Reset();
		}
	}

	Super::NativeDestruct();
}

void UWidgetProgressbar::SetViewModel(UVMProgressbar* ProgressbarViewModel)
{
	if (!IsValid(ProgressbarViewModel))
	{
		return;
	}

	if (IsValid(ViewModel))
	{
		if (PercentChangedHandle.IsValid())
		{
			ViewModel->RemoveFieldValueChangedDelegate(UVMProgressbar::FFieldNotificationClassDescriptor::GetPercent, PercentChangedHandle);
		}

		if (MaxChangedHandle.IsValid())
		{
			ViewModel->RemoveFieldValueChangedDelegate(UVMProgressbar::FFieldNotificationClassDescriptor::GetMaxValue, MaxChangedHandle);
		}
	}

	ViewModel = ProgressbarViewModel;

	MaxChangedHandle = ViewModel->AddFieldValueChangedDelegate(UVMProgressbar::FFieldNotificationClassDescriptor::GetMaxValue,
	                                                           INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(
		                                                           this, &UWidgetProgressbar::HandleMaxChanged));
	PercentChangedHandle = ViewModel->AddFieldValueChangedDelegate(UVMProgressbar::FFieldNotificationClassDescriptor::GetPercent,
	                                                               INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(
		                                                               this, &UWidgetProgressbar::HandlePercentChanged));

	RefreshMax();
	RefreshPercent();
}

void UWidgetProgressbar::HandlePercentChanged(UObject* Object, UE::FieldNotification::FFieldId Field)
{
	RefreshPercent();
}

void UWidgetProgressbar::RefreshPercent()
{
	if (IsValid(Progressbar) && IsValid(ViewModel))
	{
		Progressbar->SetPercent(ViewModel->GetPercent());
	}
}

void UWidgetProgressbar::HandleMaxChanged(UObject* Object, UE::FieldNotification::FFieldId Field)
{
	RefreshMax();
}

void UWidgetProgressbar::RefreshMax()
{
	if (IsValid(SizeBox) && IsValid(ViewModel))
	{
		const float ScaledWidth = ViewModel->GetMaxValue() * WidthPerPoint;
		SizeBox->SetWidthOverride(FMath::Clamp(ScaledWidth, MinProgressbarWidth, MaxProgressbarWidth));
	}
}

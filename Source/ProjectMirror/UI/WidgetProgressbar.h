// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetProgressbar.generated.h"

class USizeBox;
class UVMProgressbar;
class UProgressBar;
/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UWidgetProgressbar : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeDestruct() override;

protected:
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinProgressbarWidth = 10.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxProgressbarWidth = 1999.f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float WidthPerPoint = 1.0f;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> Progressbar;
	UPROPERTY()
	TObjectPtr<UVMProgressbar> ViewModel;

	void HandlePercentChanged(UObject* Object, UE::FieldNotification::FFieldId Field);
	void RefreshPercent();
	FDelegateHandle PercentChangedHandle;
	FDelegateHandle MaxChangedHandle;

	void HandleMaxChanged(UObject* Object, UE::FieldNotification::FFieldId Field);
	void RefreshMax();

public:
	void SetViewModel(UVMProgressbar* ProgressbarViewModel);
};

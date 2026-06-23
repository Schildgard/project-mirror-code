// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "WidgetButtonMenuSimple.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UWidgetButtonMenuSimple : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget), EditAnywhere)
	TObjectPtr<UCommonTextBlock> ButtonTextBlock;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> FocusLeftSound;
	
	UPROPERTY(EditAnywhere, Category = "Style")
	TSubclassOf<UCommonButtonStyle> FocusStyle;
	UPROPERTY(EditAnywhere, Category = "Style")
	TSubclassOf<UCommonButtonStyle> DefaultStyle;
	
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
};

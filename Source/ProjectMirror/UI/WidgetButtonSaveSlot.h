// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetButtonMenuSimple.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "WidgetButtonSaveSlot.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UWidgetButtonSaveSlot : public UCommonUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString ReferencedSaveGameID;

	UFUNCTION()
	void LoadReferencedSaveGame();

	UFUNCTION()
	void DeleteReferencedSaveGame();

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetButtonMenuSimple> LoadGameButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetButtonMenuSimple> DeleteButton;

	void SetSaveGameReference(const FString& SaveGameReference);
};

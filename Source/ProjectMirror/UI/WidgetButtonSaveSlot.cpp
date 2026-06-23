// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WidgetButtonSaveSlot.h"
#include "ProjectMirror/SaveSystem/SubsystemSaveAndLoad.h"


void UWidgetButtonSaveSlot::LoadReferencedSaveGame()
{
	if (USubsystemSaveAndLoad* Subsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		Subsystem->LoadSlotByID(ReferencedSaveGameID);
	}
}

void UWidgetButtonSaveSlot::DeleteReferencedSaveGame()
{
	if (USubsystemSaveAndLoad* Subsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		Subsystem->DeleteSlotByID(ReferencedSaveGameID);
	}
	RemoveFromParent();
}

void UWidgetButtonSaveSlot::SetSaveGameReference(const FString& SaveGameReference)
{
	ReferencedSaveGameID = SaveGameReference;
	LoadGameButton->OnClicked().AddUObject(this, &UWidgetButtonSaveSlot::LoadReferencedSaveGame);
	DeleteButton->OnClicked().AddUObject(this, &UWidgetButtonSaveSlot::DeleteReferencedSaveGame);
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "SubsystemObjectEvents.h"
#include "ProjectMirror/SaveSystem/SubsystemSaveAndLoad.h"


/*RegisterObject Listener is only called by Triggerable Objects*/
FSimpleMulticastDelegate* USubsystemObjectEvents::FindOrRegisterObjectEventListener(const FName& WorldEventName)
{
	return &ObjectEventListenerMap.FindOrAdd(WorldEventName);
}

/*RegisterObject Listener is only called by Interactabke Objects*/
void USubsystemObjectEvents::FireObjectEvent(const FName& ObjectEventName)
{
	/*If EventName was registered by a Listener in the same Level, this Event is to be fired*/
	if (const FSimpleMulticastDelegate* Event = ObjectEventListenerMap.Find(ObjectEventName))
	{
		Event->Broadcast();
	}
}

void USubsystemObjectEvents::RemoveObjectListenerEvent(const FName& WorldEventName)
{
	ObjectEventListenerMap.Remove(WorldEventName);
}

void USubsystemObjectEvents::UpdateObjectStatusInActivatedObjects(const FName& ObjectID)
{
	if (ActivatedObjects.Contains(ObjectID))
	{
		ActivatedObjects.Remove(ObjectID);
	}
	else
	{
		ActivatedObjects.AddUnique(ObjectID);
	}
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		SaveGameSubsystem->SaveAllData();
	}
}

void USubsystemObjectEvents::SetDroppedItemTransform(const FName& ObjectID, const FTransform& NewTransform)
{
	DroppedItemMap.Add(ObjectID, NewTransform);
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		SaveGameSubsystem->SaveAllData();
	}
}

void USubsystemObjectEvents::ClearDroppedItemTransform(const FName& ObjectID)
{
	DroppedItemMap.Remove(ObjectID);
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		SaveGameSubsystem->SaveAllData();
	}
}

bool USubsystemObjectEvents::IsObjectActivated(const FName& ObjectID) const
{
	return ActivatedObjects.Contains(ObjectID);
}

bool USubsystemObjectEvents::HasDroppedItemLocation(FName ObjectID) const
{
	return DroppedItemMap.Contains(ObjectID);
}

FTransform USubsystemObjectEvents::GetDroppedItemTransform(const FName& ObjectID) const
{
	const FTransform* SavedTransform = DroppedItemMap.Find(ObjectID);
	if (!SavedTransform)
	{
		return FTransform::Identity;
	}
	return *SavedTransform;
}

void USubsystemObjectEvents::SetObjectActivated(const FName& ObjectID, const bool bActivated)
{
	if (bActivated)
	{
		ActivatedObjects.AddUnique(ObjectID);
	}
	else
	{
		ActivatedObjects.Remove(ObjectID);
	}
}

void USubsystemObjectEvents::SetFloatState(const FName& ObjectID, const float Value)
{
	// No SaveCall! This Function happens per Frame.
	ValueStateMap.Add(ObjectID, Value);
}

bool USubsystemObjectEvents::HasFloatState(const FName& ObjectID) const
{
	return ValueStateMap.Contains(ObjectID);
}

float USubsystemObjectEvents::GetFloatState(const FName& ObjectID) const
{
	const float* SavedValue = ValueStateMap.Find(ObjectID);
	if (!SavedValue)
	{
		return 0.0f;
	}
	return *SavedValue;
}

void USubsystemObjectEvents::EmptyObjectEventListenerMap()
{
	ObjectEventListenerMap.Empty();
}

void USubsystemObjectEvents::ResetWorldState()
{
	ObjectEventListenerMap.Empty();
	ActivatedObjects.Empty();
	DroppedItemMap.Empty();
	ValueStateMap.Empty();
}

void USubsystemObjectEvents::PreloadActivatedObjectsFromSaveGame(USaveGameData* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		return;
	}
	if (FWorldEventData* WorldEventDataPtr = SaveGame->GetWorldEventData())
	{
		ActivatedObjects = WorldEventDataPtr->ActivatedObjects;
		DroppedItemMap = WorldEventDataPtr->DroppedItemMap;
		ValueStateMap = WorldEventDataPtr->ValueStateMap;
	}
}

void USubsystemObjectEvents::OnCommandLoadData_Implementation(USaveGameData* SaveGameFile)
{
	ISaveable::OnCommandLoadData_Implementation(SaveGameFile);
	ActivatedObjects = SaveGameFile->GetWorldEventData()->ActivatedObjects;
	DroppedItemMap = SaveGameFile->GetWorldEventData()->DroppedItemMap;
	ValueStateMap = SaveGameFile->GetWorldEventData()->ValueStateMap;
}

void USubsystemObjectEvents::OnCommandSaveData_Implementation(USaveGameData* SaveGameFile)
{
	ISaveable::OnCommandSaveData_Implementation(SaveGameFile);
	
	FWorldEventData WorldEventData;                                                                                                                                                                                                                                                                                                                                                                                          
	WorldEventData.ActivatedObjects = ActivatedObjects;                                                                                                                                                                                                                                                                                                                                                                      
	WorldEventData.DroppedItemMap   = DroppedItemMap;                                                                                                                                                                                                                                                                                                                                                                        
	WorldEventData.ValueStateMap    = ValueStateMap; 
	
	SaveGameFile->SetWorldEventData(WorldEventData);
}

void USubsystemObjectEvents::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<USubsystemSaveAndLoad>();
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		SaveGameSubsystem->CommandSaveGame.AddUObject(this, &USubsystemObjectEvents::OnCommandSaveData_Implementation);
		SaveGameSubsystem->CommandLoadLevelData.AddUObject(this, &USubsystemObjectEvents::OnCommandLoadData_Implementation);
	}
}

void USubsystemObjectEvents::Deinitialize()
{
	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		SaveGameSubsystem->CommandSaveGame.RemoveAll(this);
		SaveGameSubsystem->CommandLoadLevelData.RemoveAll(this);
	}

	Super::Deinitialize();
}

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

bool USubsystemObjectEvents::IsObjectActivated(const FName& ObjectID) const
{
	return ActivatedObjects.Contains(ObjectID);
}

void USubsystemObjectEvents::EmptyObjectEventListenerMap()
{
	ObjectEventListenerMap.Empty();
}

void USubsystemObjectEvents::ResetWorldState()
{
	ObjectEventListenerMap.Empty();
	ActivatedObjects.Empty();
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
	}
}

void USubsystemObjectEvents::OnCommandLoadData_Implementation(USaveGameData* SaveGameFile)
{
	ISaveable::OnCommandLoadData_Implementation(SaveGameFile);
	ActivatedObjects = SaveGameFile->GetWorldEventData()->ActivatedObjects;
}

void USubsystemObjectEvents::OnCommandSaveData_Implementation(USaveGameData* SaveGameFile)
{
	ISaveable::OnCommandSaveData_Implementation(SaveGameFile);
	const FWorldEventData& WorldEventData{ActivatedObjects};
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

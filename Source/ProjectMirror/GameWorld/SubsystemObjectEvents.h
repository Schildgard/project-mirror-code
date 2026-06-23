// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectMirror/Interfaces/Saveable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SubsystemObjectEvents.generated.h"

UCLASS()
class PROJECTMIRROR_API USubsystemObjectEvents : public UGameInstanceSubsystem, public ISaveable
{
	GENERATED_BODY()

	TMap<FName, FSimpleMulticastDelegate> ObjectEventListenerMap;
	TArray<FName> ActivatedObjects;

public:
	/*Event Delegates for Objects in the same Level. Emptry ObjectListenerMap after every LevelChange*/
	FSimpleMulticastDelegate* FindOrRegisterObjectEventListener(const FName& WorldEventName);
	void FireObjectEvent(const FName& ObjectEventName);
	void RemoveObjectListenerEvent(const FName& WorldEventName);

	void UpdateObjectStatusInActivatedObjects(const FName& ObjectID);
	bool IsObjectActivated(const FName& ObjectID) const;

	void EmptyObjectEventListenerMap();
	void PreloadActivatedObjectsFromSaveGame(USaveGameData* SaveGame);
	void ResetWorldState();

	virtual void OnCommandLoadData_Implementation(USaveGameData* SaveGameFile) override;
	virtual void OnCommandSaveData_Implementation(USaveGameData* SaveGameFile) override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};

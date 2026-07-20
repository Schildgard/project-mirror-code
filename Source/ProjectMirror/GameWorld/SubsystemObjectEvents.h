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
	TMap<FName, FTransform> DroppedItemMap;
	TMap<FName, float> ValueStateMap;

public:
	/*Event Delegates for Objects in the same Level. Empty ObjectListenerMap after every LevelChange*/
	FSimpleMulticastDelegate* FindOrRegisterObjectEventListener(const FName& WorldEventName);
	void FireObjectEvent(const FName& ObjectEventName);
	void RemoveObjectListenerEvent(const FName& WorldEventName);

	void UpdateObjectStatusInActivatedObjects(const FName& ObjectID);
	void SetDroppedItemTransform(const FName& ObjectID,const  FTransform& NewTransform);
	void ClearDroppedItemTransform(const FName& ObjectID);
	bool IsObjectActivated(const FName& ObjectID) const;
	bool HasDroppedItemLocation(FName ObjectID) const;
	FTransform GetDroppedItemTransform(const FName& ObjectID) const;
	void SetObjectActivated(const FName& ObjectID,const bool bActivated); 
	
	void  SetFloatState(const FName& ObjectID, const float Value);                                                                                                                                                                                                                                                                                                                                                                 
	bool  HasFloatState(const FName& ObjectID) const;                                                                                                                                                                                                                                                                                                                                                                              
	float GetFloatState(const FName& ObjectID) const; 

	void EmptyObjectEventListenerMap();
	void PreloadActivatedObjectsFromSaveGame(USaveGameData* SaveGame);
	void ResetWorldState();

	virtual void OnCommandLoadData_Implementation(USaveGameData* SaveGameFile) override;
	virtual void OnCommandSaveData_Implementation(USaveGameData* SaveGameFile) override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};

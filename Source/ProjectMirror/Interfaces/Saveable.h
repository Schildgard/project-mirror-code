// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectMirror/SaveSystem/SaveGameData.h"
#include "UObject/Interface.h"
#include "Saveable.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSave, Log, All);

// This class does not need to be modified.
UINTERFACE()
class USaveable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTMIRROR_API ISaveable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent)
	void OnCommandSaveData(USaveGameData* SaveGameFile);
	UFUNCTION(BlueprintNativeEvent)
	void OnCommandLoadData(USaveGameData* SaveGameFile);
};

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "Triggerable.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTriggerableObjects, Log, All);
// This class does not need to be modified.
UINTERFACE()
class UTriggerable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTMIRROR_API ITriggerable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnTriggered(const EEventReactionType ReactionType);
};

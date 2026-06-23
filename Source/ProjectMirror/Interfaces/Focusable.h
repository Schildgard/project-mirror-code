// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Focusable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFocusable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTMIRROR_API IFocusable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	 void OnFocused();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnFocusLost();
};

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Ignitable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIgnitable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTMIRROR_API IIgnitable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void Ignite()=0;
	virtual void Extinguish()=0;
	virtual bool IsIgnited()const =0;
};

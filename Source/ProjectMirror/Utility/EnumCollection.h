// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnumCollection.generated.h"


UENUM(BlueprintType)
enum class EEventReactionType : uint8
{
	Undefined,
	BecomeVisible,
	BecomeHidden,
	ToggleVisibility,
	ChangeScale,
	ActivateChildObject
};

UENUM(BlueprintType)
enum class ESoundCategory : uint8
{
	Undefined,
	BackgroundMusic,
	AmbientSound
};

UENUM(BlueprintType)
enum class ECreatureType : uint8
{
	Undefined,
	Humanoid_Normal,
	Humanoid_Armored
};

UENUM(BlueprintType)
enum class EInteractionCategory : uint8
{
	Undefined,
	Talk,
	Interact,
	Push,
	Take,
	Open,
	Close
};
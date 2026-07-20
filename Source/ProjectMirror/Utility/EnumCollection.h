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
	ActivateChildObject,
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
	Close,
	Ignite
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	Undefined,
	Head,
	Body,
	Arms,
	Legs,
	MainHand,
	OffHand,
	TwoHanded,
	BackupPrimary,
	BackupSecondary,
	BackupTwoHanded
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	Torch,
	Shortsword,
	Longsword,
	Greatsword,
	Shield
};

UENUM(BlueprintType)
enum class EInteractionHeight : uint8
{
	Low,
	Mid,
	High
};

UENUM(BlueprintType)
enum class ECharacterAnimationState : uint8
{
	Idle,
	Hanging,
	Attacking,
	Running,
};

UENUM(BlueprintType)
enum class EClimbingState : uint8
{
	None,
	Hanging,
	PullingUp
};

UENUM(BlueprintType)
enum class EDamageChannel : uint8
{
	Undefined,
	Physical,
	Magic,
	Light,
	Fire,
	Darkness
};

UENUM(BlueprintType)
enum class EWeaponActionMode : uint8
{
	Instant,
	Sustained,
	Charged,
};

UENUM(BlueprintType)
enum class ESustainedPose : uint8
{
	Guard,
	HoldForward,
	Channel
};

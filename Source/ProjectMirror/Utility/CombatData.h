// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnumCollection.h"
#include "CombatData.generated.h"

class UAnimMontage;

USTRUCT(Blueprintable)
struct FAttackStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AttackPowerPhysical = 0.0f;

	UPROPERTY(EditAnywhere)
	float AttackPowerMagic = 0.0f;

	UPROPERTY(EditAnywhere)
	float AttackPowerLight = 0.0f;

	UPROPERTY(EditAnywhere)
	float AttackPowerFire = 0.0f;

	UPROPERTY(EditAnywhere)
	float AttackPowerDarkness = 0.0f;

	UPROPERTY(EditAnywhere)
	float PoiseDamage = 0.0f;
};

USTRUCT(Blueprintable)
struct FDefenseStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float DefensePhysical = 0.0f;

	UPROPERTY(EditAnywhere)
	float DefenseMagic = 0.0f;

	UPROPERTY(EditAnywhere)
	float DefenseLight = 0.0f;

	UPROPERTY(EditAnywhere)
	float DefenseFire = 0.0f;

	UPROPERTY(EditAnywhere)
	float DefenseDarkness = 0.0f;

	UPROPERTY(EditAnywhere)
	float Poise = 0.0f;
};

USTRUCT(Blueprintable)
struct FBlockStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float PhysicalBlock = 0.0f;

	UPROPERTY(EditAnywhere)
	float MagicBlock = 0.0f;

	UPROPERTY(EditAnywhere)
	float LightBlock = 0.0f;

	UPROPERTY(EditAnywhere)
	float FireBlock = 0.0f;

	UPROPERTY(EditAnywhere)
	float DarknessBlock = 0.0f;

	UPROPERTY(EditAnywhere)
	float Stance = 0.0f;
};

USTRUCT()
struct FDamage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FAttackStats AttackStats;

	bool bIsUnblockable = false;

	float AttackDirection = 0.0f;

	float ForceValue = 0.0f;

	FVector ImpactLocation = FVector::ZeroVector;

	FVector ImpactNormal = FVector::ZeroVector;
};

USTRUCT()
struct FWeaponAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EWeaponActionMode Mode = EWeaponActionMode::Instant;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Mode != EWeaponActionMode::Sustained", EditConditionHides))
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Mode == EWeaponActionMode::Instant", EditConditionHides))
	TArray<FName> ComboSections;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Mode == EWeaponActionMode::Instant", EditConditionHides))
	bool bPreservesSustainedAction = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Mode == EWeaponActionMode::Sustained", EditConditionHides))
	ESustainedPose SustainedPose = ESustainedPose::Guard;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Mode == EWeaponActionMode::Charged", EditConditionHides))
	FName ChargeLoopSection = NAME_None;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Mode == EWeaponActionMode::Charged", EditConditionHides))
	FName ReleaseSection = NAME_None;
};

USTRUCT()
struct FCombatInputIdentity
{
	GENERATED_BODY()

	UPROPERTY()
	EEquipmentSlot Slot = EEquipmentSlot::Undefined;

	UPROPERTY()
	bool bIsHeavyVariant = false;

	bool operator==(const FCombatInputIdentity& Other) const
	{
		return Slot == Other.Slot && bIsHeavyVariant == Other.bIsHeavyVariant;
	}

	bool operator!=(const FCombatInputIdentity& Other) const { return !(*this == Other); }
};

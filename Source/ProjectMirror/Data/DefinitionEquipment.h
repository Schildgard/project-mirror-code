// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefinitionItem.h"
#include "ProjectMirror/Utility/CombatData.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "DefinitionEquipment.generated.h"


class AEquipmentSkeletal;
class AEquipmentStatic;
enum class EEquipmentSlot : uint8;


UCLASS()
class PROJECTMIRROR_API UDefinitionEquipment : public UDefinitionItem
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	EEquipmentSlot DesiredEquipmentSlot = EEquipmentSlot::Undefined;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsWeapon == true", EditConditionHides))
	EEquipmentSlot DesiredBackupSlot = EEquipmentSlot::Undefined;

	UPROPERTY(EditAnywhere)
	bool bIsWeapon = false;
	UPROPERTY(EditAnywhere)
	bool bIsFlexible = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsWeapon == true", EditConditionHides))
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsWeapon == true", EditConditionHides))
	FAttackStats AttackStats;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsWeapon == true", EditConditionHides))
	FBlockStats BlockStats;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsWeapon == false", EditConditionHides))
	FDefenseStats DefenseStats;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsWeapon == true", EditConditionHides))
	FWeaponAction NormalAction;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsWeapon == true", EditConditionHides))
	TSoftObjectPtr<USoundBase> WeaponSwingSound;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsWeapon == true", EditConditionHides))
	FWeaponAction HeavyAction;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (EditCondition = "bIsFlexible == false", EditConditionHides))
	TSubclassOf<AEquipmentStatic> StaticEquipmentClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (EditCondition = "bIsFlexible == true", EditConditionHides))
	TSubclassOf<AEquipmentSkeletal> SkeletalEquipmentClass;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsFlexible == false && bIsWeapon == false", EditConditionHides))
	FName AttachSocket = NAME_None;

	EEquipmentSlot GetEquipmentSlot() const { return DesiredEquipmentSlot; }
	EEquipmentSlot GetBackupSlot() const { return DesiredBackupSlot; }
	EWeaponType GetWeaponType() const { return WeaponType; }
	bool GetIsFlexible() const { return bIsFlexible; }
	bool GetIsWeapon() const { return bIsWeapon; }

	FAttackStats GetAttackStats() const { return AttackStats; }
	FDefenseStats GetDefenseStats() const { return DefenseStats; }
	FBlockStats GetBlockStats() const { return BlockStats; }

	FWeaponAction GetNormalAction() const { return bIsWeapon ? NormalAction : FWeaponAction(); }
	FWeaponAction GetHeavyAction() const { return bIsWeapon ? HeavyAction : FWeaponAction(); }
	
	TSoftObjectPtr<USoundBase> GetWeaponSwingSound()const {return WeaponSwingSound;}
};

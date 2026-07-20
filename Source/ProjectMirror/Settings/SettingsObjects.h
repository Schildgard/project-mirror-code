// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "SettingsObjects.generated.h"

/**
 * 
 */
UCLASS(config = ProjectMirror, DefaultConfig, meta = (DisplayName = "Objet Settings"))
class PROJECTMIRROR_API USettingsObjects : public UDeveloperSettings
{
	GENERATED_BODY()

	USettingsObjects(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Config, EditAnywhere, Category = "Drop Settings")
	float DropFallbackTimer = 3.f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Drop Settings")
	float SafeDropRadius = 20.f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Drop Settings")
	float FallbackDropForwardOffset = 60.f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Pickup Settings")                                                                                                                                                                                                                                                                                                                                                                  
	float StowBlendDuration = 0.15f;                                                                                                                                                                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                                                                                                                                                      

	UPROPERTY(Config, EditAnywhere, Category = "Drop Settings")
	TMap<EEquipmentSlot, float> AngularDampingMap;
	UPROPERTY(Config, EditAnywhere, Category = "Drop Settings")
	TMap<EEquipmentSlot, float> LinearDampingMap;

public:
	float GetStowBlendDuration() const { return StowBlendDuration; } 
	float GetSafeDropRadius() const{return SafeDropRadius;}
	float GetDropFallbackTimer() const { return DropFallbackTimer; }
	float GetFallbackDropForwardOffset() const { return FallbackDropForwardOffset; }

	float GetAngularDampingValue(const EEquipmentSlot Slot) const
	{
		if (AngularDampingMap.Contains(Slot))
		{
			return AngularDampingMap[Slot];
		}
		return 1.f;
	}

	float GetLinearDampingValue(const EEquipmentSlot Slot) const
	{
		if (LinearDampingMap.Contains(Slot))
		{
			return LinearDampingMap[Slot];
		}
		return 1.f;
	}
};

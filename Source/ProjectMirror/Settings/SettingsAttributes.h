// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SettingsAttributes.generated.h"

/**
 * 
 */
UCLASS(config = ProjectMirror, DefaultConfig, meta = (DisplayName = "Stats and Attribute Settings"))
class PROJECTMIRROR_API USettingsAttributes : public UDeveloperSettings
{
	GENERATED_BODY()
	USettingsAttributes();
public:
	UPROPERTY(Config, EditAnywhere)
	float BaseHealth = 100.f;
	
	UPROPERTY(Config, EditAnywhere)
	float BaseStamina = 100.f;
	
	UPROPERTY(Config, EditAnywhere)
	float HealthPerVitality =30.f;
	
	UPROPERTY(Config, EditAnywhere)
	float StaminaPerEndurance =10.f;
	
	UPROPERTY(Config, EditAnywhere)
	float StaminaRegenRate = 25.f;
	
	UPROPERTY(Config, EditAnywhere)
	float StaminaRegenDelay = 2.f;
	
	UPROPERTY(Config, EditAnywhere)
	float StaminaCostRunning = 3.f;
	
	UPROPERTY(Config, EditAnywhere, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ExhaustionRecoveryThreshold = 5.f;
	
};

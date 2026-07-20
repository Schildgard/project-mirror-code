// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnStaminaEmpty);
DECLARE_MULTICAST_DELEGATE(FOnStaminaChanged);

class UAttributeComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	float MaxStamina = 0.0f;
	
	UPROPERTY(VisibleAnywhere)
	float CurrentStamina = -1.0f;
	
	float TimeSinceLastDrain = 0.0f;
	
	float StaminaRegenRate = 1.0f;
	
	float StaminaDrainRate = 1.f;
	
	float ExhaustionRecoveryThreshold = 5.f;
	
	bool bIsDraining = false;
	
	float StaminaRegenDelay = 2.f;
	
	bool bIsExhausted = false;
	
	UPROPERTY()
	TObjectPtr<UAttributeComponent> CachedAttributeComponent;

	void DrainStamina(const float StaminaCost);
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FOnStaminaEmpty OnStaminaEmpty;
	FOnStaminaChanged OnStaminaChanged;
	
	void StartContinuedStaminaDrain();
	void StopDraining();
	void RegenerateStamina(float DeltaTime);
	float GetCurrentStamina() const { return CurrentStamina; }
	float GetMaxStamina() const { return MaxStamina; }
	
	bool IsExhausted() const { return bIsExhausted; }
	
	void RecalculateStamina();
};

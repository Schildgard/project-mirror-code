// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "StaminaComponent.h"

#include "AttributeComponent.h"
#include "ProjectMirror/Settings/SettingsAttributes.h"
#include "ProjectMirror/Utility/Utilities.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const USettingsAttributes* AttributeSettings = GetDefault<USettingsAttributes>())
	{
		StaminaDrainRate = AttributeSettings->StaminaCostRunning;
		StaminaRegenDelay = AttributeSettings->StaminaRegenDelay;
		StaminaRegenRate = AttributeSettings->StaminaRegenRate;
		ExhaustionRecoveryThreshold = AttributeSettings->ExhaustionRecoveryThreshold;
	}

	AActor* OwningActor = GetOwner();

	if (!IsValid(OwningActor))
	{
		return;
	}

	CachedAttributeComponent = OwningActor->FindComponentByClass<UAttributeComponent>();

	if (IsValid(CachedAttributeComponent))
	{
		RecalculateStamina();
		CachedAttributeComponent->OnAttributeChanged.AddUObject(this, &UStaminaComponent::RecalculateStamina);
	}
}


void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDraining)
	{
		DrainStamina(StaminaDrainRate * DeltaTime);
	}
	else if (TimeSinceLastDrain < StaminaRegenDelay)
	{
		TimeSinceLastDrain += DeltaTime;
	}
	else if (CurrentStamina < MaxStamina)
	{
		RegenerateStamina(DeltaTime);
	}
}

void UStaminaComponent::DrainStamina(const float StaminaCost)
{
	CurrentStamina -= StaminaCost;
	TimeSinceLastDrain = 0.0f;
	if (CurrentStamina <= 0.f)
	{
		bIsExhausted = true;
		CurrentStamina = 0.f;
		StopDraining();
		OnStaminaEmpty.Broadcast();
	}
	OnStaminaChanged.Broadcast();
}

void UStaminaComponent::StartContinuedStaminaDrain()
{
	bIsDraining = true;
}

void UStaminaComponent::StopDraining()
{
	bIsDraining = false;
}

void UStaminaComponent::RegenerateStamina(float DeltaTime)
{
	CurrentStamina += (StaminaRegenRate * DeltaTime);

	if (bIsExhausted)
	{
		if (CurrentStamina > FMath::Min (ExhaustionRecoveryThreshold, MaxStamina))
		{
			bIsExhausted = false;
		}
	}

	if (CurrentStamina >= MaxStamina)
	{
		CurrentStamina = MaxStamina;
	}
	OnStaminaChanged.Broadcast();
}

void UStaminaComponent::RecalculateStamina()
{
	
	if (!IsValid(CachedAttributeComponent))
	{
		return;
	}
	
	const USettingsAttributes* AttributeSettings = GetDefault<USettingsAttributes>();
	if (AttributeSettings)
	{
		//@todo: Cache fix Values like StaminaPerEndurance in Beginplay
		const float StaminaPerEndurance = AttributeSettings->StaminaPerEndurance;
		const float Endurance = CachedAttributeComponent->GetEffectiveAttributeValues().Endurance;
		const float BaseStamina = AttributeSettings->BaseStamina;
		const float NewMaxStamina = BaseStamina + (Endurance * StaminaPerEndurance);
		ProjectMirrorUtilities::UpdatePool(CurrentStamina, MaxStamina, NewMaxStamina);
	}

	OnStaminaChanged.Broadcast();
}

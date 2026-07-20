// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AttributeComponent.h"

#include "ProjectMirror/Settings/SettingsAttributes.h"
#include "ProjectMirror/Utility/Utilities.h"


UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	RecalculateStats();
}

float UAttributeComponent::IncreaseCurrentHealth(float Amount)
{
	const float PreviousHealth = CurrentHealth;                                                                                                                                                                                                                                                                                                                                                                                
	CurrentHealth = FMath::Min(CurrentHealth + Amount, DerivedStats.MaxHealth);                                                                                                                                                                                                                                                                                                                                                
	OnHealthChanged.Broadcast();                                                                                                                                                                                                                                                                                                                                                                                               
	return CurrentHealth - PreviousHealth; 
}

void UAttributeComponent::RecalculateStats()
{
	const float NewMaxHealth = RecalculateDerivedStats();

	ProjectMirrorUtilities::UpdatePool(CurrentHealth, DerivedStats.MaxHealth, NewMaxHealth);

	OnHealthChanged.Broadcast();
}

float UAttributeComponent::RecalculateDerivedStats()
{
	EffectiveAttributes = BaseAttributes;
	//Add Buffs when they do exist.                                                                                                                                                                                                                                                                                                                                                                                          

	const USettingsAttributes* AttributeSettings = GetDefault<USettingsAttributes>();
	// AttackPower / Defense derivation goes here later (no pool coupling)

	OnAttributeChanged.Broadcast();

	return AttributeSettings->BaseHealth + (EffectiveAttributes.Vitality * AttributeSettings->HealthPerVitality);
}

void UAttributeComponent::OverrideBaseAttributes(const FStatAttributes& NewBaseAttributes)
{
	BaseAttributes = NewBaseAttributes;
	RecalculateStats();
}

void UAttributeComponent::InitializeFromSave(const FStatAttributes& SavedBaseAttributes, const float SavedCurrentHealth)
{
	BaseAttributes = SavedBaseAttributes;
	DerivedStats.MaxHealth = RecalculateDerivedStats();

	if (SavedCurrentHealth < 0.f)
	{
		CurrentHealth = DerivedStats.MaxHealth;
		// fresh save / new game -> spawn full                                                                                                                                                                                                                                                                                                                                   
	}
	else
	{
		CurrentHealth = FMath::Clamp(SavedCurrentHealth, 0.f, DerivedStats.MaxHealth);
	}

	OnHealthChanged.Broadcast();
}

void UAttributeComponent::LoseHealth(float Health)
{
	CurrentHealth -= Health;
	if (CurrentHealth <= 0.f)
	{
		CurrentHealth = 0.f;
	}
	OnHealthChanged.Broadcast();
}

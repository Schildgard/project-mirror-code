// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttributeChanged);
DECLARE_MULTICAST_DELEGATE(FOnHealthChanged);

USTRUCT()
struct FStatAttributes
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int32 Vitality = 1;
	UPROPERTY(VisibleAnywhere)
	int32 Strength = 1;
	UPROPERTY(VisibleAnywhere)
	int32 Dexterity = 1;
	UPROPERTY(VisibleAnywhere)
	int32 Endurance = 1;
	UPROPERTY(VisibleAnywhere)
	int32 Intelligence = 1;
	UPROPERTY(VisibleAnywhere)
	int32 Faith = 1;
};


USTRUCT()
struct FDerivedStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	float MaxHealth = 0.0f;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	float CurrentHealth = -1.0f;

	UPROPERTY(VisibleAnywhere)
	FStatAttributes BaseAttributes;

	UPROPERTY(VisibleAnywhere)
	FStatAttributes EffectiveAttributes;

	UPROPERTY(VisibleAnywhere)
	FDerivedStats DerivedStats;

	float RecalculateDerivedStats();
public:
	FOnAttributeChanged OnAttributeChanged;
	FOnHealthChanged OnHealthChanged;

	FStatAttributes GetBaseAttributeValues() const { return BaseAttributes; }
	FStatAttributes GetEffectiveAttributeValues() const { return EffectiveAttributes; }
	
	void OverrideBaseAttributes(const FStatAttributes& NewBaseAttributes);
	void OverrideEffectiveAttributes(const FStatAttributes& NewEffectiveAttributes) { EffectiveAttributes = NewEffectiveAttributes; }
	
	float GetCurrentHealth() const { return CurrentHealth; }
	float GetMaxHealth() const {return DerivedStats.MaxHealth; }
	float IncreaseCurrentHealth(float Amount);
	
	bool IsAtMaxHealth() const {return FMath::IsNearlyEqual(DerivedStats.MaxHealth, CurrentHealth);}

	void RecalculateStats();
	void InitializeFromSave(const FStatAttributes& SavedBaseAttributes, const float SavedCurrentHealth);
	
	void LoseHealth(float Health);
};

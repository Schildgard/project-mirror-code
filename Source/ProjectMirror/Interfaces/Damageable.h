// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

struct FDamage;

// This class does not need to be modified.
UINTERFACE()
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by anything that can be hit. The implementer owns its own mitigation:
 * the attacker only states what the hit consists of, never how much it hurts.
 */
class PROJECTMIRROR_API IDamageable
{
	GENERATED_BODY()

public:
	virtual void ReceiveDamage(const FDamage& Damage, AActor* DamageInstigator) = 0;
};
// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include <ProjectMirror/Interfaces/Damageable.h>

#include "CoreMinimal.h"
#include "TriggerableIllusionaryObject.h"
#include "TriggerableIllusionaryHittable.generated.h"

class USoundBase;

UCLASS()
class PROJECTMIRROR_API ATriggerableIllusionaryHittable : public ATriggerableIllusionaryObject, public IDamageable
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	FName EventID = NAME_None;

	/*Preloaded in BeginPlay so the first hit does not stall on a synchronous load.*/
	UPROPERTY()
	TObjectPtr<USoundBase> FadeSound;

	virtual void BeginPlay() override;

public:
	
	virtual void ReceiveDamage(const FDamage& Damage, AActor* DamageInstigator) override;
};

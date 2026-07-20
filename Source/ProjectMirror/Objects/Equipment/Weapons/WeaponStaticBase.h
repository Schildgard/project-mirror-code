// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectMirror/Objects/Equipment/EquipmentStatic.h"
#include "WeaponStaticBase.generated.h"

class UCapsuleComponent;

UCLASS()
class PROJECTMIRROR_API AWeaponStaticBase : public AEquipmentStatic
{
	GENERATED_BODY()

public:
	AWeaponStaticBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<UCapsuleComponent> AttackHitbox;
	
	UPROPERTY()
	UAudioComponent* WeaponSwingAudioComponent;

public:
	void StartAttackFeedback();

	UCapsuleComponent* GetAttackHitbox() const { return AttackHitbox; }
	
	void PlayWeaponSwingSound();
};

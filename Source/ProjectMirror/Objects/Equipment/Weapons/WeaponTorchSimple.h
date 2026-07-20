// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeaponStaticBase.h"
#include "WeaponTorchSimple.generated.h"

class UWeaponEffectBurn;

UCLASS()
class PROJECTMIRROR_API AWeaponTorchSimple : public AWeaponStaticBase
{
	GENERATED_BODY()

public:
	AWeaponTorchSimple();

protected:
	
	UPROPERTY(EditAnywhere)
	UWeaponEffectBurn* BurnEffect;
	
public:
	virtual void OnSlotChanged(const EEquipmentSlot NewSlot) override;
};

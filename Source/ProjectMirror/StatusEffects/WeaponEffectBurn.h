// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeaponEffect.h"
#include "ProjectMirror/Interfaces/Ignitable.h"
#include "WeaponEffectBurn.generated.h"


class UPointLightComponent;
class USphereComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UWeaponEffectBurn : public UWeaponEffect, public IIgnitable
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	UPointLightComponent*  LightSource;
	
	virtual void BeginPlay() override;

public:
	UWeaponEffectBurn();
	
	virtual void Ignite() override;
	virtual void Extinguish() override;
	virtual bool IsIgnited()const override {return bIsEffectActive;}
	
};


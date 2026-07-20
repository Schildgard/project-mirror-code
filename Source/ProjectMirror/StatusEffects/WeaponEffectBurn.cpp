// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WeaponEffectBurn.h"

#include "Components/PointLightComponent.h"


UWeaponEffectBurn::UWeaponEffectBurn()
{
	PrimaryComponentTick.bCanEverTick = false;
	LightSource = CreateDefaultSubobject<UPointLightComponent>("LightSource");
}

void UWeaponEffectBurn::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(LightSource))
	{
		if (const AActor* OwningActor = GetOwner())
		{
			UMeshComponent* OwnerMesh = OwningActor->GetComponentByClass<UMeshComponent>();
			if (IsValid(OwnerMesh))
			{
				LightSource->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, EffectSocketName);
			}
		}
	}
}

void UWeaponEffectBurn::Ignite()
{
	SetEffectActive(true);
	if (IsValid(LightSource))
	{
		LightSource->SetVisibility(true);
	}
}

void UWeaponEffectBurn::Extinguish()
{
	SetEffectActive(false);
	if (IsValid(LightSource))
	{
		LightSource->SetVisibility(false);
	}
}

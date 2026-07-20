// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WeaponTorchSimple.h"

#include "ProjectMirror/Data/DefinitionEquipment.h"
#include "ProjectMirror/StatusEffects/WeaponEffectBurn.h"


AWeaponTorchSimple::AWeaponTorchSimple()
{
	PrimaryActorTick.bCanEverTick = false;
	BurnEffect = CreateDefaultSubobject<UWeaponEffectBurn> (TEXT("BurnEffect"));
	BurnEffect->SetupAttachment(RootComponent);
	BurnEffect->SetEffectActive(false);
}

void AWeaponTorchSimple::OnSlotChanged(const EEquipmentSlot NewSlot)
{
	Super::OnSlotChanged(NewSlot);
	const bool bIsBackUpSlot = NewSlot == EEquipmentSlot::BackupPrimary || NewSlot == EEquipmentSlot::BackupSecondary || NewSlot == EEquipmentSlot::BackupTwoHanded;
	if (bIsBackUpSlot)
	{
		if (IsValid(BurnEffect) && BurnEffect->IsIgnited())
		{
		BurnEffect->Extinguish();
		}
	}
}


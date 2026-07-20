// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "SettingsObjects.h"

USettingsObjects::USettingsObjects(const FObjectInitializer& ObjectInitializer)
{
	CategoryName = TEXT("Project Mirror");
	AngularDampingMap.Add(EEquipmentSlot::Head, 1.f);
	AngularDampingMap.Add(EEquipmentSlot::Body, 1.f);
	AngularDampingMap.Add(EEquipmentSlot::Legs, 1.f);
	AngularDampingMap.Add(EEquipmentSlot::Arms, 1.f);
	AngularDampingMap.Add(EEquipmentSlot::MainHand, 1.f);
	AngularDampingMap.Add(EEquipmentSlot::OffHand, 1.f);
	AngularDampingMap.Add(EEquipmentSlot::TwoHanded, 1.f);
}

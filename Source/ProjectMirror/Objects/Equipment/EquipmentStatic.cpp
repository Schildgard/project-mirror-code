// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "EquipmentStatic.h"

// Sets default values
AEquipmentStatic::AEquipmentStatic()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractableObjectStatic.h"


// Sets default values
AInteractableObjectStatic::AInteractableObjectStatic()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(StaticMeshComponent);
	SnapTargetComponent->SetupAttachment(RootComponent);
}

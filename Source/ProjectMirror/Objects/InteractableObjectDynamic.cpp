// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractableObjectDynamic.h"

AInteractableObjectDynamic::AInteractableObjectDynamic()
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SetRootComponent(SkeletalMeshComponent);
	SnapTargetComponent->SetupAttachment(RootComponent);
}

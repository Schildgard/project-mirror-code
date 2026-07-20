// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "EquipmentSkeletal.h"



AEquipmentSkeletal::AEquipmentSkeletal()
{
	PrimaryActorTick.bCanEverTick = true;
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(SkeletalMesh);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEquipmentSkeletal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEquipmentSkeletal::AttachToOwner(USkeletalMeshComponent* OwnerMesh, const FName& Socket)
{
	Super::AttachToOwner(OwnerMesh, NAME_None);                                                                                                                                                                                                                                                                                                                         
	if (IsValid(OwnerMesh))                                                                                                                                                                                                                                                                                                                                                                                                  
	{                                                                                                                                                                                                                                                                                                                                                                                                                        
		SkeletalMesh->SetLeaderPoseComponent(OwnerMesh);                                                                                                                                                                                                                                                                                                                                                                 
	}  
}

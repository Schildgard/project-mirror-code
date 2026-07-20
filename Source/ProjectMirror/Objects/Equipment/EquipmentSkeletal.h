// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentBase.h"
#include "EquipmentSkeletal.generated.h"

UCLASS()
class PROJECTMIRROR_API AEquipmentSkeletal : public AEquipmentBase
{
	GENERATED_BODY()

public:
	AEquipmentSkeletal();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void AttachToOwner(USkeletalMeshComponent* OwnerMesh, const FName& Socket) override;
	virtual UMeshComponent* GetMesh() const override { return SkeletalMesh; }
};

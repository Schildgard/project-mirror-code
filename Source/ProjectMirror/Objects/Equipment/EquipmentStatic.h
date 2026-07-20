// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentBase.h"
#include "GameFramework/Actor.h"
#include "EquipmentStatic.generated.h"

class UDefinitionEquipment;
struct FEquipmentBundle;

UCLASS()
class PROJECTMIRROR_API AEquipmentStatic : public AEquipmentBase
{
	GENERATED_BODY()

protected:
	AEquipmentStatic();

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

public:
	virtual UMeshComponent* GetMesh() const override { return StaticMesh; }
};

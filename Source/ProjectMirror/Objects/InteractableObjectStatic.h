// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectBase.h"
#include "InteractableObjectStatic.generated.h"

UCLASS()
class PROJECTMIRROR_API AInteractableObjectStatic : public AInteractableObjectBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UStaticMeshComponent* StaticMeshComponent;

public:
	AInteractableObjectStatic();
};

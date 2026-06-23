// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectBase.h"
#include "InteractableObjectDynamic.generated.h"

UCLASS()
class PROJECTMIRROR_API AInteractableObjectDynamic : public AInteractableObjectBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	USkeletalMeshComponent* SkeletalMeshComponent;

public:
	AInteractableObjectDynamic();
};

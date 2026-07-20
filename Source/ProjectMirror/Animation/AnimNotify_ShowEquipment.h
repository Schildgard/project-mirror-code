// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_HideEquipment.h"
#include "AnimNotify_ShowEquipment.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API UAnimNotify_ShowEquipment : public UAnimNotify_HideEquipment
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

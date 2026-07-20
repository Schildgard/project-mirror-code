// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "AnimNotifyState_Attack.generated.h"

/**
 * Marks the active part of an attack animation. Holds no state of its own:
 * notify instances are shared, so everything runtime lives on the CombatComponent.
 */
UCLASS()
class PROJECTMIRROR_API UAnimNotifyState_Attack : public UAnimNotifyState
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	EEquipmentSlot AttackingSlot = EEquipmentSlot::MainHand;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override; 
};

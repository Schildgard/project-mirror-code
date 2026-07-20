// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimNotify_Climb.h"

#include "ProjectMirror/Characters/Components/ClimbComponent.h"

void UAnimNotify_Climb::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	const AActor* OwningActor = MeshComp->GetOwner();
	if (!IsValid(OwningActor))
	{
		return;
	}

	UClimbComponent* ClimbComponent = OwningActor->FindComponentByClass<UClimbComponent>();
	if (!IsValid(ClimbComponent))
	{
		return;
	}
	ClimbComponent->SnapOnClimbable();
}

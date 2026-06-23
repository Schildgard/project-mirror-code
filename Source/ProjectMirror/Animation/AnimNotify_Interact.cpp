// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimNotify_Interact.h"

#include "ProjectMirror/InteractionComponent.h"

void UAnimNotify_Interact::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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
	UInteractionComponent* InteractionComponent = OwningActor->GetComponentByClass<UInteractionComponent>();
	if (!IsValid(InteractionComponent))
	{
		return;
	}
	InteractionComponent->Interact();
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimNotify_Footstep.h"
#include "ProjectMirror/Characters/Components/FootstepAudioComponent.h"

UAnimNotify_Footstep::UAnimNotify_Footstep()
{
#if WITH_EDITOR
	bShouldFireInEditor = false;
#endif
	
}

void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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
	
	 UFootstepAudioComponent* FootstepAudio = OwningActor->GetComponentByClass<UFootstepAudioComponent>();
	if (!IsValid(FootstepAudio))
	{
		return;
	}
	FootstepAudio->PlayFootstepAudio();
	
}

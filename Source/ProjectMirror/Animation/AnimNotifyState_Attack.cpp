// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimNotifyState_Attack.h"
#include "ProjectMirror/Characters/Components/CombatComponent.h"
#include "ProjectMirror/Utility/Utilities.h"

void UAnimNotifyState_Attack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                          const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	UCombatComponent* CombatComponent = ProjectMirrorUtilities::GetOwnerComponentFromMesh<UCombatComponent>(MeshComp);
	if (!IsValid(CombatComponent))
	{
		return;
	}
	CombatComponent->BeginAttackWindow(AttackingSlot);
}

void UAnimNotifyState_Attack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	UCombatComponent* CombatComponent = ProjectMirrorUtilities::GetOwnerComponentFromMesh<UCombatComponent>(MeshComp);
	if (!IsValid(CombatComponent))
	{
		return;
	}

	CombatComponent->EndAttackWindow();
}

void UAnimNotifyState_Attack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	        Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);                                                                                                                                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                                                                                                                                                                                                                 
        if (!IsValid(MeshComp))                                                                                                                                                                                                                                                                                                                                                                                                  
        {                                                                                                                                                                                                                                                                                                                                                                                                                        
                return;                                                                                                                                                                                                                                                                                                                                                                                                          
        }                                                                                                                                                                                                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                                                                                                                                                                 
        UCombatComponent* CombatComponent = ProjectMirrorUtilities::GetOwnerComponentFromMesh<UCombatComponent>(MeshComp);                                                                                                                                                                                                                                                                                                       
        if (!IsValid(CombatComponent))                                                                                                                                                                                                                                                                                                                                                                                           
        {                                                                                                                                                                                                                                                                                                                                                                                                                        
                return;                                                                                                                                                                                                                                                                                                                                                                                                          
        }                                                                                                                                                                                                                                                                                                                                                                                                                        
                                                                                                                                                                                                                                                                                                                                                                                                                                 
        CombatComponent->TickAttackTrace();
	
}

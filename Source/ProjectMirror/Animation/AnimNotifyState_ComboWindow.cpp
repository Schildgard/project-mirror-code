// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimNotifyState_ComboWindow.h"

#include "ProjectMirror/Characters/Components/CombatComponent.h"
#include "ProjectMirror/Utility/Utilities.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
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

	CombatComponent->OpenComboWindow();
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	CombatComponent->CloseComboWindow();
}

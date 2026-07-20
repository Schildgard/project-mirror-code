// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimNotify_ShowEquipment.h"
#include "ProjectMirror/Characters/Components/EquipmentComponent.h"

void UAnimNotify_ShowEquipment::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(MeshComp))
	{
		return;
	}

	const AActor* Owner = MeshComp->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UEquipmentComponent* EquipmentComponent = Owner->GetComponentByClass<UEquipmentComponent>();
	if (IsValid(EquipmentComponent))
	{
		EquipmentComponent->CommitPickupVisuals();
	}
}

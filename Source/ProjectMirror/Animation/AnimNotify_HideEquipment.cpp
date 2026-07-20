// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimNotify_HideEquipment.h"
#include "ProjectMirror/Characters/Components/EquipmentComponent.h"
#include "ProjectMirror/Objects/Equipment/EquipmentBase.h"

void UAnimNotify_HideEquipment::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
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
	if (!IsValid(EquipmentComponent))
	{
		return;
	}
	
	 TMap<EEquipmentSlot, FEquipmentBundle> EquipmentToChangeVisibility = EquipmentComponent->GetHiddenEquipmentMap();
	
	for (auto& EquipmentBundle : EquipmentToChangeVisibility)
	{
		if (!IsValid(EquipmentBundle.Value.EquipmentActor))
		{
			continue;
		}
		if ( UMeshComponent* EquipmentMesh = EquipmentBundle.Value.EquipmentActor->GetMesh())
		{
			EquipmentMesh->SetVisibility(false);
		}
	}
	
}

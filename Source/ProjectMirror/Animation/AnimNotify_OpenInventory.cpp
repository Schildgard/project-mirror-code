// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimNotify_OpenInventory.h"

#include "ProjectMirror/ControllerPlayable.h"
#include "ProjectMirror/UI/HUDInGame.h"

void UAnimNotify_OpenInventory::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	AActor* OwningActor = MeshComp->GetOwner();

	if (!IsValid(OwningActor))
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OwningActor);

	if (!IsValid(Pawn))
	{
		return;
	}
	
	AControllerPlayable* Controller = Cast<AControllerPlayable>(Pawn->GetController());
	if (!IsValid(Controller))
	{
		return;
	}
	
	Controller->SpawnSitCameraRig();
	
	AHUDInGame* InGameHUD = Cast<AHUDInGame>(Controller->GetHUD());
	
	if (!IsValid(InGameHUD))
	{
		return;
	}
	
	InGameHUD->PushInGameMenu();
}

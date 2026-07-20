// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "TriggerableIllusionaryObject.h"


ATriggerableIllusionaryObject::ATriggerableIllusionaryObject()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ATriggerableIllusionaryObject::BeginPlay()
{
	Super::BeginPlay();
	if (!IsValid(MeshComponent))
	{
		return;
	}

	const int32 MaterialCount = MeshComponent->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = MeshComponent->CreateDynamicMaterialInstance(MaterialIndex);

		if (!IsValid(DynamicMaterialInstance))
		{
			continue;
		}

		FLinearColor BaseColor = FLinearColor::White;
		if (!DynamicMaterialInstance->GetVectorParameterValue(TEXT("BaseColor"), BaseColor))
		{
			UE_LOG(LogTriggerableObjects, Warning, TEXT("%s: Material slot %d has no 'BaseColor' parameter — it will not blend."), *GetName(), MaterialIndex)
			continue;
		}
		DynamicMaterialInstances.Add(DynamicMaterialInstance);
		MaterialBaseColors.Add(BaseColor);
	}
	if (!MaterialBaseColors.IsEmpty())
	{
		CurrentAlphaValue = MaterialBaseColors[0].A;
	}

	if (bIsCurrentlyBlending)
	{
		FinishBlending();
	}
}

void ATriggerableIllusionaryObject::ApplyAlphaToMaterials()
{
	for (int32 MaterialIndex = 0; MaterialIndex < DynamicMaterialInstances.Num(); ++MaterialIndex)
	{
		FLinearColor BlendedColor = MaterialBaseColors[MaterialIndex];
		BlendedColor.A = CurrentAlphaValue;
		DynamicMaterialInstances[MaterialIndex]->SetVectorParameterValue(TEXT("BaseColor"), BlendedColor);
	}
}

void ATriggerableIllusionaryObject::FinishBlending()
{
	CurrentAlphaValue = DesiredAlphaValue;
	ApplyAlphaToMaterials();
	bIsCurrentlyBlending = false;
	SetActorTickEnabled(false);

	if (IsValid(MeshComponent))
	{
		MeshComponent->SetVisibility(bShouldBeVisible, true);
		MeshComponent->SetCollisionEnabled(bShouldBeVisible ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}

void ATriggerableIllusionaryObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsCurrentlyBlending || !IsValid(MeshComponent) || DynamicMaterialInstances.IsEmpty())
	{
		return;
	}

	CurrentAlphaValue = FMath::FInterpTo(CurrentAlphaValue, DesiredAlphaValue, DeltaTime, BlendingSpeed);

	if (FMath::IsNearlyEqual(CurrentAlphaValue, DesiredAlphaValue, 0.01f))
	{
		FinishBlending();
		return;
	}

	ApplyAlphaToMaterials();
}

void ATriggerableIllusionaryObject::BecomeVisible()
{
	if (!IsValid(MeshComponent))
	{
		return;
	}
	SetActorTickEnabled(true);
	MeshComponent->SetVisibility(true, true);

	DesiredAlphaValue = 1.0f;
	bShouldBeVisible = true;
	bIsCurrentlyBlending = true;
}

void ATriggerableIllusionaryObject::BecomeInvisible()
{
	if (!IsValid(MeshComponent))
	{
		return;
	}
	SetActorTickEnabled(true);
	DesiredAlphaValue = 0.0f;
	bShouldBeVisible = false;
	bIsCurrentlyBlending = true;
}

void ATriggerableIllusionaryObject::OnTriggered_Implementation(const EEventReactionType ReactionType)
{
	switch (ReactionType)
	{
	case EEventReactionType::BecomeVisible:
		BecomeVisible();
		break;
	case EEventReactionType::BecomeHidden:
		BecomeInvisible();
		break;
	default:
		return;
	}
}

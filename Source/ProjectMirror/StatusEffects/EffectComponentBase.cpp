// Copyright 2026 Leonard Kemenani. All Rights Reserved.
#include "EffectComponentBase.h"


UEffectComponentBase::UEffectComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;

	VisualEffectComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraEffect");
	VisualEffectComponent->bAutoActivate = bIsEffectActive;
}

void UEffectComponentBase::BeginPlay()
{
	Super::BeginPlay();
	if (const AActor* OwningActor = GetOwner())
	{
		UMeshComponent* OwnerMesh = OwningActor->GetComponentByClass<UMeshComponent>();
		if (IsValid(OwnerMesh))
		{
			VisualEffectComponent->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, EffectSocketName);
		}
	}
	if (bIsEffectActive)
	{
		OnActivated();
	}
	else
	{
		OnDeactivated();
	}
}

void UEffectComponentBase::SetEffectActive(const bool bActive)
{
	if (bIsEffectActive != bActive)
	{
		bIsEffectActive = bActive;
		if (bIsEffectActive)
		{
			OnActivated();
		}
		else
		{
			OnDeactivated();
		}
	}
}

void UEffectComponentBase::OnActivated()
{
	if (IsValid(VisualEffectComponent))
	{
		VisualEffectComponent->SetActive(true);
	}
}

void UEffectComponentBase::OnDeactivated()
{
	if (IsValid(VisualEffectComponent))
	{
		VisualEffectComponent->SetActive(false);
	}
}

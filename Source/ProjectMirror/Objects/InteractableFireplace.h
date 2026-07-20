// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObjectStatic.h"
#include "ProjectMirror/Interfaces/Ignitable.h"
#include "InteractableFireplace.generated.h"

class UPointLightComponent;
class UNiagaraComponent;

UCLASS()
class PROJECTMIRROR_API AInteractableFireplace : public AInteractableObjectStatic, public IIgnitable
{
	GENERATED_BODY()

public:
	AInteractableFireplace();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Activated")
	bool bIsIgnited = false;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* NiagaraComponent;
	
	UPROPERTY(EditAnywhere)
	UPointLightComponent*  LightSource;

public:
	virtual void Ignite() override;
	virtual void Extinguish() override;
	virtual bool IsIgnited() const override { return bIsIgnited; }
	IIgnitable* ResolveIgnitableWeapon(UInteractionComponent* InteractionComponent) const;

	virtual bool IsInterActionPossible_Implementation(UInteractionComponent* InteractionComponent) override;
	virtual void OnInteract_Implementation(UInteractionComponent* InteractionComponent) override;
};

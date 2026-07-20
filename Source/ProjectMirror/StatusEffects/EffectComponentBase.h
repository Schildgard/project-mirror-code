// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "EffectComponentBase.generated.h"

static constexpr const TCHAR* EffectSocketName = TEXT("EffectSocket");
UCLASS(Abstract)
class PROJECTMIRROR_API UEffectComponentBase : public USceneComponent
{
	GENERATED_BODY()
protected:
	
	UEffectComponentBase();
	virtual void BeginPlay()override;
	
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* VisualEffectComponent;
	
	UPROPERTY(EditAnywhere)
	bool bIsEffectActive = true;

public:
	void SetEffectActive(const bool bActive);
	bool GetEffectActive()const {return bIsEffectActive;};
	
	virtual void OnActivated();
	virtual void OnDeactivated();
};

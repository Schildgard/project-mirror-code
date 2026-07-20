// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EffectComponentBase.h"
#include "CharacterEffect.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UCharacterEffect : public UEffectComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterEffect();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};

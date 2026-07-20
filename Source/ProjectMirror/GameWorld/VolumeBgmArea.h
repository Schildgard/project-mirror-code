// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "VolumeBgmArea.generated.h"

UCLASS(Abstract)
class PROJECTMIRROR_API AVolumeBgmArea : public ATriggerBox
{
	GENERATED_BODY()
	
	
protected:
	AVolumeBgmArea();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> AreaBgm;
	
	virtual void BeginPlay() override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};

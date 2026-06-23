// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "VolumeLevelTransition.generated.h"

UCLASS()
class PROJECTMIRROR_API AVolumeLevelTransition : public ATriggerBox
{
	GENERATED_BODY()

public:
	AVolumeLevelTransition();

protected:
	
	UPROPERTY(EditAnywhere)
	FName LevelToLoadName= NAME_None;
	
	UPROPERTY(EditAnywhere)
	FName EntryPointID = NAME_None;
	
public:
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};

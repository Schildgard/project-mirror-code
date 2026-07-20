// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectMirror/Interfaces/Triggerable.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "TriggerableObject.generated.h"

USTRUCT()
struct FEventReactionConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EEventReactionType EventReaction = EEventReactionType::Undefined;
	
	UPROPERTY(EditAnywhere)
	bool bCheckObjectConditionOnBeginPlay = false;
	
	UPROPERTY(EditAnywhere)
	bool bRegisterOnEvent = true;
};

UCLASS()
class PROJECTMIRROR_API ATriggerableObject : public AActor, public ITriggerable
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TMap<FName, FEventReactionConfig> EventMap;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	ATriggerableObject();
	virtual void BeginPlay() override;

public:
	virtual void OnTriggered_Implementation(const EEventReactionType EventReaction) override;
};

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnumCollection.h"
#include "NiagaraSystem.h"
#include "HitFeedbackData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECTMIRROR_API FHitFeedbackEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNiagaraSystem> Effect;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundBase> Sound;
};

USTRUCT()
struct FResolvedHitFeedback
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> Effect;

	UPROPERTY()
	TObjectPtr<USoundBase> Sound;
};

USTRUCT(BlueprintType)
struct PROJECTMIRROR_API FHitFeedbackChannelSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TMap<EDamageChannel, FHitFeedbackEntry> ChannelFeedbackMap;
};

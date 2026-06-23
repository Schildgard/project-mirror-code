#pragma once

#include "CoreMinimal.h"
#include "FootstepSurfaceSounds.generated.h"

USTRUCT(BlueprintType)
struct FFootstepSurfaceSounds
{
	GENERATED_BODY()
	
	UPROPERTY(Config, EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<USoundBase>> FootstepAudioMap;
};

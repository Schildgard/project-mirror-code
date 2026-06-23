// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "FootstepAudioComponent.generated.h"

enum class ECreatureType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UFootstepAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

public:
	UFootstepAudioComponent();

protected:
	
	EPhysicalSurface CurrentSurfaceType = SurfaceType1;
	
	UPROPERTY()
	TMap<TEnumAsByte<EPhysicalSurface>,TObjectPtr<USoundBase>> CachedFootstepSoundMap;
	
	virtual void BeginPlay() override;
	
	float LineTraceDistance = 30.f;
	
	void ChangeSurfaceType(const EPhysicalSurface NewSurfaceType);
	void RebuildSoundMap(const ECreatureType CreatureType);
public:
	void PlayFootstepAudio();
	void CheckSurfaceAndSetSound();
	
	void OnCreatureTypeChanged(ECreatureType NewCreatureType);
};

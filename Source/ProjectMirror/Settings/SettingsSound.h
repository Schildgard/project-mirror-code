// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProjectMirror/Utility/FootstepSurfaceSounds.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "SettingsSound.generated.h"


UCLASS(config = ProjectMirror, DefaultConfig, meta = (DisplayName = " Sound Settings"))
class PROJECTMIRROR_API USettingsSound : public UDeveloperSettings
{
	GENERATED_BODY()
	USettingsSound(const FObjectInitializer& InitializerModule);
public:
	UPROPERTY(Config, EditAnywhere)
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<USoundBase>> LevelMusicMap;
	
	UPROPERTY(Config, EditAnywhere)
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<USoundBase>> LevelAmbientMap;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<USoundBase> IngGameMenuMusic;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<USoundBase> RestRoomMusic;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<USoundClass> DefaultSoundClass;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<USoundBase> ButtonFocusLeftSound;

	/*Played when a hittable illusionary object starts fading out after being struck.*/
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<USoundBase> IllusionaryObjectFadeSound;

	UPROPERTY(Config, EditAnywhere)
	float SoundFadeDuration = 3.f;
	
	UPROPERTY(Config, EditAnywhere)
	TMap<ECreatureType, FFootstepSurfaceSounds> FootstepAudioMap;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<USoundAttenuation> WeaponSwingAttenuation;
	
};

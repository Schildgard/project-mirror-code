// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "FootstepAudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectMirror/Characters/CharacterBase.h"
#include "ProjectMirror/Settings/SettingsSound.h"


UFootstepAudioComponent::UFootstepAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = false;
}


void UFootstepAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	ECreatureType CurrentCreatureType = ECreatureType::Humanoid_Normal;
	if (ACharacterBase* CharacterBase = Cast<ACharacterBase>(GetOwner()))
	{
		CurrentCreatureType = CharacterBase->GetCreatureType();
	}
	RebuildSoundMap(CurrentCreatureType);
}

void UFootstepAudioComponent::ChangeSurfaceType(const EPhysicalSurface NewSurfaceType)
{
	TObjectPtr<USoundBase>* Found = CachedFootstepSoundMap.Find(NewSurfaceType);
	if (!Found || !IsValid(*Found))
	{
		return;
	}

	Sound = *Found;
	CurrentSurfaceType = NewSurfaceType;
}

void UFootstepAudioComponent::RebuildSoundMap(const ECreatureType CreatureType)
{
	if (const USettingsSound* SoundSettings = GetDefault<USettingsSound>())
	{
		CachedFootstepSoundMap.Reset();
		const FFootstepSurfaceSounds* SurfaceSoundMapPtr = SoundSettings->FootstepAudioMap.Find(CreatureType);
		if (!SurfaceSoundMapPtr)
		{
			return;
		}

		for (const auto& SurfaceSound : SurfaceSoundMapPtr->FootstepAudioMap)
		{
			CachedFootstepSoundMap.Add(SurfaceSound.Key, SurfaceSound.Value.LoadSynchronous());
		}

		if (CachedFootstepSoundMap.IsEmpty())
		{
			return;
		}

		TObjectPtr<USoundBase>* Found = CachedFootstepSoundMap.Find(CurrentSurfaceType);
		if (!Found || !IsValid(*Found))
		{
			return;
		}

		Sound = *Found;
	}
}

void UFootstepAudioComponent::PlayFootstepAudio()
{
	CheckSurfaceAndSetSound();

	if (!IsValid(Sound))
	{
		return;
	}
	Play();
}

void UFootstepAudioComponent::CheckSurfaceAndSetSound()
{
	const ETraceTypeQuery TraceTypeQuery = TraceTypeQuery1;
	const FVector FootLocation = GetComponentLocation();
	const FVector LineTraceTargetPos{FootLocation.X, FootLocation.Y, FootLocation.Z - LineTraceDistance};
	const TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(this, FootLocation, LineTraceTargetPos, TraceTypeQuery, true, ActorsToIgnore, EDrawDebugTrace::None, HitResult,
	                                      true);

	if (HitResult.PhysMaterial.IsValid())
	{
		UPhysicalMaterial* SurfaceMaterial = HitResult.PhysMaterial.Get();
		if (IsValid(SurfaceMaterial))
		{
			if (SurfaceMaterial->SurfaceType == CurrentSurfaceType)
			{
				return;
			}
			ChangeSurfaceType(SurfaceMaterial->SurfaceType);
		}
	}
}

void UFootstepAudioComponent::OnCreatureTypeChanged(ECreatureType NewCreatureType)
{
	RebuildSoundMap(NewCreatureType);
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "WeaponStaticBase.h"

#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectMirror/Settings/SettingsSound.h"


AWeaponStaticBase::AWeaponStaticBase()
{
	AttackHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackHitbox"));
	AttackHitbox->SetupAttachment(StaticMesh);

	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitbox->SetGenerateOverlapEvents(false);

	WeaponSwingAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponSwingAudio"));
	WeaponSwingAudioComponent->SetupAttachment(StaticMesh);
	WeaponSwingAudioComponent->bAutoActivate = false;
}

void AWeaponStaticBase::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(WeaponSwingAudioComponent))
	{
		if (IsValid(SourceDefinition))
		{
			USoundBase* LoadedSound = SourceDefinition->GetWeaponSwingSound().LoadSynchronous();
			if (IsValid(LoadedSound))
			{
				WeaponSwingAudioComponent->SetSound(LoadedSound);
			}
		}
		if (const USettingsSound* SoundSettings = GetDefault<USettingsSound>())
		{
			if (USoundAttenuation* SoundAttenuation = SoundSettings->WeaponSwingAttenuation.LoadSynchronous())
			{
				WeaponSwingAudioComponent->SetAttenuationSettings(SoundAttenuation);
			}
		}
	}
}

void AWeaponStaticBase::StartAttackFeedback()
{
	PlayWeaponSwingSound();
}

void AWeaponStaticBase::PlayWeaponSwingSound()
{
	if (!IsValid(WeaponSwingAudioComponent))
	{
		return;
	}
	WeaponSwingAudioComponent->Play();
}

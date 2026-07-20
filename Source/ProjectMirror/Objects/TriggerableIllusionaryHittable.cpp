// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "TriggerableIllusionaryHittable.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"
#include "ProjectMirror/Settings/SettingsSound.h"
#include "ProjectMirror/Utility/CombatData.h"
#include "ProjectMirror/Utility/Utilities.h"

void ATriggerableIllusionaryHittable::BeginPlay()
{
	Super::BeginPlay();

	if (EventID.IsNone() || !EventMap.Contains(EventID))
	{
		UE_LOG(LogTriggerableObjects, Warning, TEXT("%s: EventID '%s' has no matching entry in EventMap — the hidden state will not survive loading."),
		       *GetName(), *EventID.ToString())
	}

	if (const USettingsSound* SoundSettings = GetDefault<USettingsSound>())
	{
		FadeSound = SoundSettings->IllusionaryObjectFadeSound.LoadSynchronous();
	}
}

void ATriggerableIllusionaryHittable::ReceiveDamage(const FDamage& Damage, AActor* DamageInstigator)
{
	if (!ProjectMirrorUtilities::IsAPlayerCharacter(DamageInstigator))
	{
		return;
	}

	/*Captured before triggering: further hits during the fade must not restart the sound.*/
	const bool bStartsFadingNow = bShouldBeVisible;

	if (!EventID.IsNone())
	{
		USubsystemObjectEvents* SubsystemObjectEvents = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>();
		if (IsValid(SubsystemObjectEvents))
		{
			SubsystemObjectEvents->SetObjectActivated(EventID, true);
		}
	}
	Execute_OnTriggered(this, EEventReactionType::BecomeHidden);

	if (!bStartsFadingNow || !IsValid(FadeSound))
	{
		return;
	}

	const FVector SoundLocation = Damage.ImpactLocation.IsNearlyZero() ? GetActorLocation() : Damage.ImpactLocation;
	UGameplayStatics::PlaySoundAtLocation(this, FadeSound, SoundLocation);
}

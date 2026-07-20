#include "Utilities.h"

#include "HitFeedbackData.h"
#include "NiagaraFunctionLibrary.h"

void ProjectMirrorUtilities::PlayHitFeedback(UWorld* World, const FResolvedHitFeedback& Feedback, const FVector& ImpactLocation, const FVector& ImpactNormal)
{
	if (!IsValid(World))
	{
		return;
	}

	if (IsValid(Feedback.Effect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Feedback.Effect, ImpactLocation, ImpactNormal.Rotation());
	}

	if (IsValid(Feedback.Sound))
	{
		UGameplayStatics::PlaySoundAtLocation(World, Feedback.Sound, ImpactLocation);
	}
}

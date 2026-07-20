#pragma once

#include "CoreMinimal.h"
#include "HitFeedbackData.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "ProjectMirror/Utility/CombatData.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "ProjectMirror/ControllerPlayable.h"

namespace ProjectMirrorUtilities
{
	template <typename TInterface>
	AActor* GetClosestActorWithInterface(const AActor* CallingActor, const float ScanRadius)
	{
		const FVector& InvocatorLocation = CallingActor->GetActorLocation();
		static const TArray<TEnumAsByte<EObjectTypeQuery>> ScannableActorTypes{
			UEngineTypes::ConvertToObjectType(ECC_WorldStatic), UEngineTypes::ConvertToObjectType(ECC_WorldDynamic), UEngineTypes::ConvertToObjectType(ECC_Pawn)
		};
		TArray<FHitResult> HitResults;
		/*Scan for Actors in Range*/
		UKismetSystemLibrary::SphereTraceMultiForObjects(CallingActor->GetWorld(), InvocatorLocation, InvocatorLocation, ScanRadius, ScannableActorTypes, false,
		                                                 TArray<AActor*>{}, EDrawDebugTrace::ForDuration, HitResults, true);

		if (HitResults.IsEmpty())
		{
			return nullptr;
		}

		/*Check Actors for DesiredInterface*/
		TArray<AActor*> ActorWithInterface;
		ActorWithInterface.Reserve(HitResults.Num());
		TSet<AActor*> ActorsToIgnore;
		ActorsToIgnore.Reserve(HitResults.Num());

		for (const FHitResult& Hit : HitResults)
		{
			AActor* Actor = Hit.GetActor();
			if (!IsValid(Actor) || ActorsToIgnore.Contains(Actor))
			{
				continue;
			}
			ActorsToIgnore.Add(Actor);
			if (Actor->Implements<TInterface>())
			{
				ActorWithInterface.Add(Actor);
			}
		}
		if (ActorWithInterface.IsEmpty())
		{
			return nullptr;
		}

		/*Get Closest Actor*/
		float OutDistance = FLT_MAX;
		AActor* ClosestActor = UGameplayStatics::FindNearestActor(InvocatorLocation, ActorWithInterface, OutDistance);

		return ClosestActor;
	}

	inline bool IsAPlayerCharacter(const AActor* ActorToCheck)
	{
		if (!IsValid(ActorToCheck))
		{
			return false;
		}

		if (const APawn* Pawn = Cast<APawn>(ActorToCheck))
		{
			if (Cast<AControllerPlayable>(Pawn->GetController()))
			{
				return true;
			}
		}
		return false;
	}

	inline void UpdatePool(float& CurrentValue, float& OldMax, const float NewMax)
	{
		if (CurrentValue < 0.0f)
		{
			CurrentValue = NewMax;
		}
		else if (NewMax > OldMax)
		{
			CurrentValue += (NewMax - OldMax);
		}

		OldMax = NewMax;
		CurrentValue = FMath::Clamp(CurrentValue, 0.0f, NewMax);
	}

	template <typename TComponent>
	TComponent* GetOwnerComponentFromMesh(const USkeletalMeshComponent* MeshComp)
	{
		if (!IsValid(MeshComp))
		{
			return nullptr;
		}

		const AActor* OwningActor = MeshComp->GetOwner();
		if (!IsValid(OwningActor))
		{
			return nullptr;
		}

		return OwningActor->GetComponentByClass<TComponent>();
	}

	inline EDamageChannel ResolveDominantDamageChannel(const FAttackStats& AttackStats)
	{
		EDamageChannel DominantChannel = EDamageChannel::Physical;
		float HighestValue = AttackStats.AttackPowerPhysical;

		if (AttackStats.AttackPowerMagic > HighestValue)
		{
			DominantChannel = EDamageChannel::Magic;
			HighestValue = AttackStats.AttackPowerMagic;
		}
		if (AttackStats.AttackPowerLight > HighestValue)
		{
			DominantChannel = EDamageChannel::Light;
			HighestValue = AttackStats.AttackPowerLight;
		}
		if (AttackStats.AttackPowerFire > HighestValue)
		{
			DominantChannel = EDamageChannel::Fire;
			HighestValue = AttackStats.AttackPowerFire;
		}
		if (AttackStats.AttackPowerDarkness > HighestValue)
		{
			DominantChannel = EDamageChannel::Darkness;
			HighestValue = AttackStats.AttackPowerDarkness;
		}
		return HighestValue > 0.0f ? DominantChannel : EDamageChannel::Undefined;
	}

	void PlayHitFeedback(UWorld* World, const FResolvedHitFeedback& Feedback, const FVector& ImpactLocation, const FVector& ImpactNormal);

	inline FResolvedHitFeedback ResolveHitFeedbackEntry(const FHitFeedbackEntry& SourceEntry)
	{
		FResolvedHitFeedback ResolvedFeedback;
		ResolvedFeedback.Effect = SourceEntry.Effect.LoadSynchronous();
		ResolvedFeedback.Sound = SourceEntry.Sound.LoadSynchronous();
		return ResolvedFeedback;
	}

	template <typename TKey>
	void ResolveHitFeedbackMap(const TMap<TKey, FHitFeedbackEntry>& SourceMap, TMap<TKey, FResolvedHitFeedback>& OutResolvedMap)
	{
		OutResolvedMap.Reset();
		for (const TPair<TKey, FHitFeedbackEntry>& SourceEntry : SourceMap)
		{
			OutResolvedMap.Add(SourceEntry.Key, ResolveHitFeedbackEntry(SourceEntry.Value));
		}
	}

	template <typename TKey>
	void PlayHitFeedbackForKey(UWorld* World, const TMap<TKey, FResolvedHitFeedback>& ResolvedMap, const TKey& Key,
	                           const FResolvedHitFeedback& FallbackFeedback, const FVector& ImpactLocation, const FVector& ImpactNormal)
	{
		const FResolvedHitFeedback* FeedbackEntry = ResolvedMap.Find(Key);
		if (!FeedbackEntry)
		{
			FeedbackEntry = &FallbackFeedback;
		}

		PlayHitFeedback(World, *FeedbackEntry, ImpactLocation, ImpactNormal);
	}
}

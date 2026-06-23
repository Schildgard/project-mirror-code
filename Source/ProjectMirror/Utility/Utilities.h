#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectMirror/ControllerPlayable.h"

namespace ProjectMirrorUtilities
{
	template <typename TInterface> AActor* GetClosestActorWithInterface(const AActor* CallingActor, const float ScanRadius)
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
			if (Cast<AControllerPlayable>(Pawn->GetController()) )
			{
				return true;
			}
		}
		return false;
	}
}

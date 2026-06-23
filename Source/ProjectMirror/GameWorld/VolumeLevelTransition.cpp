// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "VolumeLevelTransition.h"

#include "ProjectMirror/SubsystemLevelLoading.h"
#include "ProjectMirror/Utility/Utilities.h"


AVolumeLevelTransition::AVolumeLevelTransition()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVolumeLevelTransition::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		if (USubsystemLevelLoading* LevelLoadSubsystem = GetGameInstance()->GetSubsystem<USubsystemLevelLoading>())
		{
			if (!EntryPointID.IsNone())
			{
				LevelLoadSubsystem->SetPendingEntryPoint(EntryPointID);
			}
			LevelLoadSubsystem->EnterNewLevel(LevelToLoadName);
		}
	}
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "VolumeBgmArea.h"
#include "SubsystemBgm.h"
#include "ProjectMirror/Utility/Utilities.h"


AVolumeBgmArea::AVolumeBgmArea()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVolumeBgmArea::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverlaps();
}

void AVolumeBgmArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!IsValid(AreaBgm) || !IsValid(OtherActor) || !ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		return;
	}
	if (USubsystemBgm* BgmSubsystem = GetGameInstance()->GetSubsystem<USubsystemBgm>())
	{
		BgmSubsystem->EnterBgmArea(AreaBgm, ESoundCategory::BackgroundMusic);
	}
}

void AVolumeBgmArea::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	if (!IsValid(AreaBgm) || !IsValid(OtherActor) || !ProjectMirrorUtilities::IsAPlayerCharacter(OtherActor))
	{
		return;
	}
	if (USubsystemBgm* BgmSubsystem = GetGameInstance()->GetSubsystem<USubsystemBgm>())
	{
		BgmSubsystem->ExitBgmArea(AreaBgm, ESoundCategory::BackgroundMusic);
	}
}

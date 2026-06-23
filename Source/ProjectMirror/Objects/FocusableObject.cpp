// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "FocusableObject.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"


AFocusableObject::AFocusableObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFocusableObject::BeginPlay()
{
	Super::BeginPlay();
	if (FocusEventID.IsNone())
	{
		return;
	}
	if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		EventSubsystem->FindOrRegisterObjectEventListener(FocusEventID);
		EventSubsystem->FindOrRegisterObjectEventListener(FocusLostEventID);
	}
}

void AFocusableObject::OnFocused_Implementation()
{
	if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		EventSubsystem->FireObjectEvent(FocusEventID);
	}
}

void AFocusableObject::OnFocusLost_Implementation()
{
	if (USubsystemObjectEvents* EventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		EventSubsystem->FireObjectEvent(FocusLostEventID);
	}
}


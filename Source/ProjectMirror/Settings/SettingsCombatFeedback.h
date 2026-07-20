// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProjectMirror/Utility/HitFeedbackData.h"
#include "SettingsCombatFeedback.generated.h"

/**
 * 
 */
UCLASS(config = ProjectMirror, DefaultConfig, meta = (DisplayName = "Combat Feedback Settings"))
class PROJECTMIRROR_API USettingsCombatFeedback : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USettingsCombatFeedback();
	UPROPERTY(Config, EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, FHitFeedbackEntry> SurfaceFeedbackMap;

	UPROPERTY(Config, EditAnywhere)
	FHitFeedbackEntry DefaultSurfaceFeedback;

	UPROPERTY(Config, EditAnywhere)
	TMap<ECreatureType, FHitFeedbackChannelSet> TargetFeedbackMap;

	UPROPERTY(Config, EditAnywhere)
	FHitFeedbackEntry DefaultTargetFeedback;
};

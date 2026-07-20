// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TriggerableObject.h"
#include "TriggerableIllusionaryObject.generated.h"

UCLASS()
class PROJECTMIRROR_API ATriggerableIllusionaryObject : public ATriggerableObject
{
	GENERATED_BODY()

public:
	ATriggerableIllusionaryObject();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterialInstances;
	
	TArray<FLinearColor> MaterialBaseColors;
	
	float CurrentAlphaValue = 1.0f; 
	
	bool bShouldBeVisible = true;
	bool bIsCurrentlyBlending = false;
	float DesiredAlphaValue = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Blend")
	float BlendingSpeed = 1.0;
	
	void ApplyAlphaToMaterials();                                                                                                                                                                                                                                                                                                                                                                                                  
	void FinishBlending();

public:
	virtual void Tick(float DeltaTime) override;

	void BecomeVisible();
	void BecomeInvisible();

	virtual void OnTriggered_Implementation(const EEventReactionType ReactionType) override;
	
};

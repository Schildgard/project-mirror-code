// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RunComponent.generated.h"

class UStaminaComponent;
class UCharacterMovementComponent;
class UAnimInstanceSimple;

DECLARE_MULTICAST_DELEGATE(FOnRunStarted);
DECLARE_MULTICAST_DELEGATE(FOnRunEnded);
UCLASS()
class PROJECTMIRROR_API URunComponent : public UActorComponent
{
	GENERATED_BODY()


protected:
	UPROPERTY()
	UAnimInstanceSimple* CachedAnimInstance;
	
	UPROPERTY()
	UCharacterMovementComponent* CachedMovementComponent;
	
	UPROPERTY()
	UStaminaComponent* CachedStaminaComponent;
	
	URunComponent();
	virtual void BeginPlay() override;

	bool bIsRunning = false;

public:
	
	FOnRunStarted OnRunStarted;
	FOnRunEnded OnRunEnded;

	bool GetIsRunning() const { return bIsRunning; };
	
	void StartRunning();
	void CancelRunning();
	
	void CacheStaminaComponent(UStaminaComponent* StaminaComponent){ CachedStaminaComponent = StaminaComponent; };
};

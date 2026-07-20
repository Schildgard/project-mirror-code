// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "ClimbComponent.generated.h"

struct FInputActionValue;
class UCharacterMovementComponent;

DECLARE_MULTICAST_DELEGATE(FOnClimbStart);
DECLARE_MULTICAST_DELEGATE(FOnClimbEnd);
DECLARE_MULTICAST_DELEGATE(FOnClimbUpStart);

USTRUCT()
struct FClimbData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* ClimbableActor = nullptr;

	UPROPERTY()
	FVector ClimbNormal = FVector::ZeroVector;

	UPROPERTY()
	FVector ClimbLocation = FVector::ZeroVector;

	UPROPERTY()
	bool bClimbable = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMIRROR_API UClimbComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClimbComponent();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> CachedOwnerCharacter = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> CachedMovementComponent = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> CachedOwnerMesh = nullptr;
	
	UPROPERTY()
	FClimbData CachedClimbData;
	
	EClimbingState CurrentClimbingState = EClimbingState::None;

	void CheckClimbableDistance(FClimbData& OutClimbData);
	void CheckClimbableHeight(FClimbData& OutClimbData);

public:
	
	FOnClimbStart OnClimbStart;
	FOnClimbEnd OnClimbEnd;
	FOnClimbUpStart OnClimbUpStart;
	
	void HangOnClimbable();
	void CancelClimbing();
	
	void SnapOnClimbable();
	void SnapOnLedgeSurface();
	
	void TryPlayClimbAnimation();
	void PullUpFromLedge();
	
	void HandleClimbingInput(const FInputActionValue& Input);
	
	void CompleteClimbing();
};

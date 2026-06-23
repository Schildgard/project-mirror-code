// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"


struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTMIRROR_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULockOnComponent();

protected:
	
	UPROPERTY(EditAnywhere, Category = "Test")
	bool bUseCameraPos= false;
	
	UPROPERTY()
	TObjectPtr<AActor> FocusedTarget;
	
	UPROPERTY()
	TObjectPtr<APlayerController> CachedPlayerController;
	
	UPROPERTY()
	TObjectPtr<USpringArmComponent> CachedCameraArmComponent;
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> CachedCameraComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LockOn, meta = (AllowPrivateAccess))
	TArray<TEnumAsByte<EObjectTypeQuery>> ValidObjectTypes;
	
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void FocusOnTarget(AActor* TargetActor);
	
	UFUNCTION()
	void ToggleLockOn();
	
	UFUNCTION()
	TArray<AActor*> ScanForValidTargets()const;
	
	UFUNCTION()
	void SwitchLockOnTarget(const FInputActionValue& InputValue);

		
};

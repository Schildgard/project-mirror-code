// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ControllerPlayable.generated.h"

class ACameraRig;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class PROJECTMIRROR_API AControllerPlayable : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/* Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LockOnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SwitchLockOnTargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> OpenMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ChangeWeaponAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ClimbAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ClimbMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))                                                                                                                                                                                                                                                                                                                             
	TObjectPtr<UInputMappingContext> ClimbMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))                                                                                                                                                                                                                                                                                                                             
	TObjectPtr<UInputAction> DiscardMainWeaponAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))                                                                                                                                                                                                                                                                                                                             
	TObjectPtr<UInputAction> DiscardOffWeaponAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))                                                                                                                                                                                                                                                                                                                             
	TObjectPtr<UInputAction> MainWeaponAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))                                                                                                                                                                                                                                                                                                                             
	TObjectPtr<UInputAction> MainWeaponHeavyAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))                                                                                                                                                                                                                                                                                                                             
	TObjectPtr<UInputAction> OffWeaponAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))                                                                                                                                                                                                                                                                                                                             
	TObjectPtr<UInputAction> OffWeaponHeavyAction;
	
	UFUNCTION()
	void SetupPlayerInput();

	void SetupLockOnComponent(APawn* InPawn);
	void SetupInteractionComponent(APawn* InPawn);
	void SetupCameraComponent(APawn* InPawn);
	void SetupEquipmentInput(APawn* InPawn);
	void SetupClimbComponent(APawn* InPawn);
	void SetupStaminaComponent(APawn* InPawn);
	void SetupRunComponent(APawn* InPawn);
	void SetupCombatInput(APawn* InPawn);

	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY()
	FTimerHandle ViewTargetBlendTimerHandle;
	
	UPROPERTY()
	TObjectPtr<ACameraRig> CachedBlendCamera = nullptr;
	
	UPROPERTY()
	TObjectPtr<AActor> CachedDefaultCamera = nullptr;

public:
	UInputMappingContext* GetDefaultMappingContext() const { return DefaultMappingContext; }

	void BindPlayerHUD();
	void OnViewTargetBlendFinish();
	void OnInGameMenuOpened();

	void SpawnSitCameraRig();
	void RemoveSitCameraRig();

	void EnterClimbInputMode();  // Default raus, Climb rein                                                                                                                                                                                                                                                                                                                                                                       
	void ExitClimbInputMode();   // Climb raus, Default rein  
};

// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "ControllerPlayable.h"
#include "Characters/CharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InteractionComponent.h"
#include "LockOnComponent.h"
#include "Animation/AnimInstanceSimple.h"
#include "Camera/CameraComponent.h"
#include "Characters/Components/ClimbComponent.h"
#include "Characters/Components/CombatComponent.h"
#include "Characters/Components/EquipmentComponent.h"
#include "Characters/Components/RunComponent.h"
#include "Characters/Components/StaminaComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Settings/SettingsCamera.h"
#include "Settings/SettingsCharacter.h"
#include "UI/HUDInGame.h"
#include "Utility/CameraRig.h"

void AControllerPlayable::SetupPlayerInput()
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetPawn()->InputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		return;
	}

	ACharacterBase* PlayerCharacter = Cast<ACharacterBase>(GetPawn());
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, PlayerCharacter, &ACharacterBase::Move);
	}
	if (JumpAction)
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, PlayerCharacter, &ACharacterBase::Jump);
	}
	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, PlayerCharacter, &ACharacterBase::Look);
	}
	if (InteractAction)
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, PlayerCharacter, &ACharacterBase::TriggerInteraction);
	}


	if (UAnimInstanceSimple* AnimInstanceSimple = Cast<UAnimInstanceSimple>(PlayerCharacter->GetMesh()->GetAnimInstance()))
	{
		if (OpenMenuAction)
		{
			EnhancedInputComponent->BindAction(OpenMenuAction, ETriggerEvent::Triggered, AnimInstanceSimple,
			                                   &UAnimInstanceSimple::TryPlayRestingAnimationMontage);
		}
	}
}

void AControllerPlayable::SetupLockOnComponent(APawn* InPawn)
{
	if (ULockOnComponent* LockOnComponent = Cast<ULockOnComponent>(
		InPawn->AddComponentByClass(ULockOnComponent::StaticClass(), false, FTransform::Identity, false)))
	{
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetPawn()->InputComponent);
		if (IsValid(EnhancedInputComponent))
		{
			EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, LockOnComponent,
			                                   &ULockOnComponent::ToggleLockOn);
			EnhancedInputComponent->BindAction(SwitchLockOnTargetAction, ETriggerEvent::Triggered, LockOnComponent,
			                                   &ULockOnComponent::SwitchLockOnTarget);
		}
	}
}

void AControllerPlayable::SetupInteractionComponent(APawn* InPawn)
{
	if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(
		InPawn->AddComponentByClass(UInteractionComponent::StaticClass(), false, FTransform::Identity, false)))
	{
		InPawn->AddInstanceComponent(InteractionComponent);
	}
}

void AControllerPlayable::SetupCameraComponent(APawn* InPawn)
{
	if (!IsValid(InPawn))
	{
		return;
	}

	float CameraArmDistance = 400.f;
	FVector CameraOffset = FVector::ZeroVector;
	bool CameraArmUsesPawnControlRotation = true;
	bool CameraUsesPawnControlRotation = false;

	if (const USettingsCharacter* CharacterSettings = GetDefault<USettingsCharacter>())
	{
		CameraArmDistance = CharacterSettings->CameraDistance;
		CameraOffset = CharacterSettings->CameraOffset;
		CameraArmUsesPawnControlRotation = CharacterSettings->CameraArmUsesPawnControlRotation;
		CameraUsesPawnControlRotation = CharacterSettings->CameraUsesPawnControlRotation;
	}

	if (USpringArmComponent* CameraArmComponent = Cast<USpringArmComponent>(
		InPawn->AddComponentByClass(USpringArmComponent::StaticClass(), false, FTransform::Identity, false)))
	{
		InPawn->AddInstanceComponent(CameraArmComponent);
		CameraArmComponent->TargetArmLength = CameraArmDistance;
		CameraArmComponent->bUsePawnControlRotation = CameraArmUsesPawnControlRotation;
		if (UCameraComponent* Camera = Cast<UCameraComponent>(InPawn->AddComponentByClass(UCameraComponent::StaticClass(), false, FTransform::Identity, false)))
		{
			Camera->AttachToComponent(CameraArmComponent, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
			Camera->bUsePawnControlRotation = CameraUsesPawnControlRotation;
			Camera->SetRelativeLocation(CameraOffset);
			InPawn->AddInstanceComponent(Camera);
		}
	}
}

void AControllerPlayable::SetupEquipmentInput(APawn* InPawn)
{
	if (!IsValid(InPawn))
	{
		return;
	}
	UEquipmentComponent* EquipmentComponent = InPawn->FindComponentByClass<UEquipmentComponent>();
	if (!IsValid(EquipmentComponent))
	{
		return;
	}
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InPawn->InputComponent);
	if (IsValid(EnhancedInputComponent))
	{
		if (ChangeWeaponAction)
		{
			EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, EquipmentComponent, &UEquipmentComponent::HandleSwitchWeaponInput);
		}
		if (DiscardMainWeaponAction)
		{
			EnhancedInputComponent->BindAction(DiscardMainWeaponAction, ETriggerEvent::Triggered, EquipmentComponent, &UEquipmentComponent::DiscardMainWeapon);
		}
		if (DiscardOffWeaponAction)
		{
			EnhancedInputComponent->BindAction(DiscardOffWeaponAction, ETriggerEvent::Triggered, EquipmentComponent, &UEquipmentComponent::DiscardOffWeapon);
		}
	}
}

void AControllerPlayable::SetupClimbComponent(APawn* InPawn)
{
	if (!IsValid(InPawn))
	{
		return;
	}
	UClimbComponent* ClimbComponent = Cast<UClimbComponent>(InPawn->AddComponentByClass(UClimbComponent::StaticClass(), false, FTransform::Identity, false));
	if (!IsValid(ClimbComponent))
	{
		return;
	}

	ClimbComponent->OnClimbStart.AddUObject(this, &AControllerPlayable::EnterClimbInputMode);
	ClimbComponent->OnClimbEnd.AddUObject(this, &AControllerPlayable::ExitClimbInputMode);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InPawn->InputComponent);
	if (IsValid(EnhancedInputComponent))
	{
		if (IsValid(ClimbAction))
		{
			EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Triggered, ClimbComponent, &UClimbComponent::HangOnClimbable);
			EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Canceled, ClimbComponent, &UClimbComponent::CancelClimbing);
			EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Completed, ClimbComponent, &UClimbComponent::CancelClimbing);
		}
		if (IsValid(ClimbMoveAction))
		{
			EnhancedInputComponent->BindAction(ClimbMoveAction, ETriggerEvent::Triggered, ClimbComponent, &UClimbComponent::HandleClimbingInput);
		}
	}
}

void AControllerPlayable::SetupStaminaComponent(APawn* InPawn)
{
	if (!IsValid(InPawn))
	{
		return;
	}
	if (UStaminaComponent* StaminaComponent = Cast<UStaminaComponent>(
		InPawn->AddComponentByClass(UStaminaComponent::StaticClass(), false, FTransform::Identity, false)))
	{
		InPawn->AddInstanceComponent(StaminaComponent);
	}
}

void AControllerPlayable::SetupRunComponent(APawn* InPawn)
{
	if (!IsValid(InPawn))
	{
		return;
	}
	if (URunComponent* RunComponent = Cast<URunComponent>(InPawn->AddComponentByClass(URunComponent::StaticClass(), false, FTransform::Identity, false)))
	{
		InPawn->AddInstanceComponent(RunComponent);
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InPawn->InputComponent))
		{
			if (IsValid(SprintAction))
			{
				EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, RunComponent, &URunComponent::StartRunning);
				EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, RunComponent, &URunComponent::CancelRunning);
				EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, RunComponent, &URunComponent::CancelRunning);
			}
		}

		if (UStaminaComponent* StaminaComponent = InPawn->FindComponentByClass<UStaminaComponent>())
		{
			RunComponent->OnRunStarted.AddUObject(StaminaComponent, &UStaminaComponent::StartContinuedStaminaDrain);
			RunComponent->OnRunEnded.AddUObject(StaminaComponent, &UStaminaComponent::StopDraining);
			StaminaComponent->OnStaminaEmpty.AddUObject(RunComponent, &URunComponent::CancelRunning);
			RunComponent->CacheStaminaComponent(StaminaComponent);
		}
	}
}

void AControllerPlayable::SetupCombatInput(APawn* InPawn)
{
	if (!IsValid(InPawn))
	{
		return;
	}

	UCombatComponent* CombatComponent = InPawn->FindComponentByClass<UCombatComponent>();
	if (!IsValid(CombatComponent))
	{
		return;
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InPawn->InputComponent);
	if (!IsValid(EnhancedInputComponent))
	{
		return;
	}

	if (MainWeaponAction)
	{
		EnhancedInputComponent->BindAction(MainWeaponAction, ETriggerEvent::Started, CombatComponent, &UCombatComponent::PerformRightWeaponNormalAction);
		EnhancedInputComponent->BindAction(MainWeaponAction, ETriggerEvent::Completed, CombatComponent, &UCombatComponent::ReleaseRightWeaponNormalAction);
		EnhancedInputComponent->BindAction(MainWeaponAction, ETriggerEvent::Canceled, CombatComponent, &UCombatComponent::ReleaseRightWeaponNormalAction);
	}
	if (MainWeaponHeavyAction)
	{
		EnhancedInputComponent->BindAction(MainWeaponHeavyAction, ETriggerEvent::Started, CombatComponent, &UCombatComponent::PerformRightWeaponHeavyAction);
		EnhancedInputComponent->BindAction(MainWeaponHeavyAction, ETriggerEvent::Completed, CombatComponent, &UCombatComponent::ReleaseRightWeaponHeavyAction);
		EnhancedInputComponent->BindAction(MainWeaponHeavyAction, ETriggerEvent::Canceled, CombatComponent, &UCombatComponent::ReleaseRightWeaponHeavyAction);
	}
	if (OffWeaponAction)
	{
		EnhancedInputComponent->BindAction(OffWeaponAction, ETriggerEvent::Started, CombatComponent, &UCombatComponent::PerformLeftWeaponNormalAction);
		EnhancedInputComponent->BindAction(OffWeaponAction, ETriggerEvent::Completed, CombatComponent, &UCombatComponent::ReleaseLeftWeaponNormalAction);
		EnhancedInputComponent->BindAction(OffWeaponAction, ETriggerEvent::Canceled, CombatComponent, &UCombatComponent::ReleaseLeftWeaponNormalAction);
	}
	if (OffWeaponHeavyAction)
	{
		EnhancedInputComponent->BindAction(OffWeaponHeavyAction, ETriggerEvent::Started, CombatComponent, &UCombatComponent::PerformLeftWeaponHeavyAction);
		EnhancedInputComponent->BindAction(OffWeaponHeavyAction, ETriggerEvent::Completed, CombatComponent, &UCombatComponent::ReleaseLeftWeaponHeavyAction);
		EnhancedInputComponent->BindAction(OffWeaponHeavyAction, ETriggerEvent::Canceled, CombatComponent, &UCombatComponent::ReleaseLeftWeaponHeavyAction);
	}
}

void AControllerPlayable::OnPossess(APawn* InPawn)
{
	if (!IsValid(InPawn))
	{
		return;
	}

	Super::OnPossess(InPawn);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	SetupPlayerInput();
	SetupCameraComponent(InPawn);
	SetupLockOnComponent(InPawn);
	SetupInteractionComponent(InPawn);

	SetupEquipmentInput(InPawn);
	SetupCombatInput(InPawn);

	SetupClimbComponent(InPawn);

	//Stamina Component must be initialized before run component 
	SetupStaminaComponent(InPawn);
	SetupRunComponent(InPawn);

	BindPlayerHUD();
}

void AControllerPlayable::BindPlayerHUD()
{
	AHUDInGame* PlayerHUD = Cast<AHUDInGame>(GetHUD());
	UInteractionComponent* InteractionComponent = GetPawn()->FindComponentByClass<UInteractionComponent>();

	if (!IsValid(PlayerHUD))
	{
		return;
	}

	PlayerHUD->OnInGameMenuOpened.AddUObject(this, &AControllerPlayable::OnInGameMenuOpened);
	PlayerHUD->OnInGameMenuClosed.AddUObject(this, &AControllerPlayable::RemoveSitCameraRig);

	if (IsValid(InteractionComponent))
	{
		InteractionComponent->OnCurrentInteractableChanged.AddUObject(PlayerHUD, &AHUDInGame::OnInteractableChanged);
	}

	PlayerHUD->BindPlayerComponents(GetPawn());
}

void AControllerPlayable::OnViewTargetBlendFinish()
{
	SetInputMode(FInputModeGameOnly{});
}

void AControllerPlayable::OnInGameMenuOpened()
{
	SetInputMode(FInputModeUIOnly{});
}

void AControllerPlayable::SpawnSitCameraRig()
{
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
	{
		return;
	}
	USkeletalMeshComponent* CharacterMesh = ControlledPawn->FindComponentByClass<USkeletalMeshComponent>();
	if (!IsValid(CharacterMesh))
	{
		return;
	}

	if (IsValid(CachedBlendCamera))
	{
		CachedBlendCamera->Destroy();
		CachedBlendCamera = nullptr;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = ControlledPawn;

	ACameraRig* CameraRig = GetWorld()->SpawnActor<ACameraRig>(SpawnInfo);
	if (IsValid(CameraRig))
	{
		USpringArmComponent* CameraArm = CameraRig->GetCameraArm();
		UCameraComponent* Camera = CameraRig->GetCamera();

		if (!IsValid(Camera) || !IsValid(CameraArm))
		{
			return;
		}
		CachedBlendCamera = CameraRig;
		CameraRig->AttachToComponent(ControlledPawn->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		if (const USettingsCamera* CameraSettings = GetDefault<USettingsCamera>())
		{
			CachedDefaultCamera = GetViewTarget();
			CachedBlendCamera->ApplyPose(CameraSettings->SitCameraPivotOffset, CameraSettings->SitCameraArmRotation, CameraSettings->SitCameraArmLength);
			SetViewTargetWithBlend(CameraRig, CameraSettings->SitCameraBlendTime, VTBlend_EaseInOut, 2.0f, true);
		}
	}
}

void AControllerPlayable::RemoveSitCameraRig()
{
	if (!IsValid(CachedBlendCamera) || !IsValid(CachedDefaultCamera))
	{
		return;
	}
	if (const USettingsCamera* CameraSettings = GetDefault<USettingsCamera>())
	{
		SetViewTargetWithBlend(CachedDefaultCamera, CameraSettings->SitCameraBlendTime, VTBlend_EaseInOut, 2.0f, true);
		GetWorldTimerManager().SetTimer(ViewTargetBlendTimerHandle, this, &AControllerPlayable::OnViewTargetBlendFinish, CameraSettings->SitCameraBlendTime);
	}

	CachedBlendCamera->Destroy();
}

void AControllerPlayable::EnterClimbInputMode()
{
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem))
	{
		return;
	}
	if (DefaultMappingContext)
	{
		InputSubsystem->RemoveMappingContext(DefaultMappingContext);
	}
	if (ClimbMappingContext)
	{
		InputSubsystem->AddMappingContext(ClimbMappingContext, 0);
	}
}

void AControllerPlayable::ExitClimbInputMode()
{
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!IsValid(InputSubsystem))
	{
		return;
	}
	if (ClimbMappingContext)
	{
		InputSubsystem->RemoveMappingContext(ClimbMappingContext);
	}
	if (DefaultMappingContext)
	{
		InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

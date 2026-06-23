// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "ControllerPlayable.h"
#include "Characters/CharacterBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InteractionComponent.h"
#include "LockOnComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Settings/SettingsCharacter.h"
#include "UI/HUDInGame.h"


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

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, PlayerCharacter, &ACharacterBase::Move);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, PlayerCharacter, &ACharacterBase::Jump);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, PlayerCharacter, &ACharacterBase::Look);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, PlayerCharacter, &ACharacterBase::Run);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, PlayerCharacter,
	                                   &ACharacterBase::CancelRunning);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, PlayerCharacter,
	                                   &ACharacterBase::CancelRunning);
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, PlayerCharacter, &ACharacterBase::TriggerInteraction);

	if (AHUDInGame* PlayerHUD = Cast<AHUDInGame>(GetHUD()))
	{
		EnhancedInputComponent->BindAction(OpenMenuAction, ETriggerEvent::Triggered, PlayerHUD, &AHUDInGame::PushInGameMenu);
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
		}
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
	BindPlayerHUD();
}

void AControllerPlayable::BindPlayerHUD()
{
	AHUDInGame* PlayerHUD = Cast<AHUDInGame>(GetHUD());
	UInteractionComponent* InteractionComponent = GetPawn()->FindComponentByClass<UInteractionComponent>();

	if (IsValid(InteractionComponent) && IsValid(PlayerHUD))
	{
		InteractionComponent->OnCurrentInteractableChanged.AddUObject(PlayerHUD, &AHUDInGame::OnInteractableChanged);
	}
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#include "CharacterBase.h"

#include "../Animation/AnimInstanceSimple.h"
#include "Components/FootstepAudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectMirror/InteractionComponent.h"
#include "ProjectMirror/SaveSystem/SubsystemSaveAndLoad.h"
#include "ProjectMirror/Settings/SettingsCharacter.h"

DEFINE_LOG_CATEGORY(LogCharacter);

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;


	FootstepAudioComponent = CreateDefaultSubobject<UFootstepAudioComponent>(FName("FootstepAudioComponent"));
	FootstepAudioComponent->SetupAttachment(GetMesh(), TEXT("Root"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (USubsystemSaveAndLoad* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USubsystemSaveAndLoad>())
	{
		//@todo: does not override potential blueprint overrides, wrap in function to fix.
		SaveGameSubsystem->CommandSaveGame.AddUObject(this, &ACharacterBase::OnCommandSaveData_Implementation);
	}
	SetupMovementSettings();
}

void ACharacterBase::Run()
{
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (!IsValid(MovementComponent))
	{
		return;
	}
	if (const USettingsCharacter* CharacterSettings = GetDefault<USettingsCharacter>())
	{
		MovementComponent->MaxWalkSpeed = CharacterSettings->DefaultRunningSpeed;
	}
}

void ACharacterBase::CancelRunning()
{
	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (!IsValid(MovementComponent))
	{
		return;
	}
	if (const USettingsCharacter* CharacterSettings = GetDefault<USettingsCharacter>())
	{
		MovementComponent->MaxWalkSpeed = CharacterSettings->DefaultWalkingSpeed;
	}
}

void ACharacterBase::SetStrafing(const bool IsStrafing) const
{
	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (IsValid(CharacterMovementComponent))
	{
		CharacterMovementComponent->bOrientRotationToMovement = !IsStrafing;
	}

	UAnimInstanceSimple* AnimationInstance = Cast<UAnimInstanceSimple>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimationInstance))
	{
		AnimationInstance->bIsStrafing = IsStrafing;
	}
}

void ACharacterBase::SetupMovementSettings()
{
	if (const USettingsCharacter* CharacterSettings = GetDefault<USettingsCharacter>())
	{
		bUseControllerRotationPitch = CharacterSettings->bUseControllerRotationPitch;
		bUseControllerRotationYaw = CharacterSettings->bUseControllerRotationYaw;
		bUseControllerRotationRoll = CharacterSettings->bUseControllerRotationRoll;

		GetCharacterMovement()->bOrientRotationToMovement = CharacterSettings->bOrientRotationToMovement;
		GetCharacterMovement()->RotationRate = CharacterSettings->RotationRate;

		GetCharacterMovement()->JumpZVelocity = CharacterSettings->JumpZVelocity;
		GetCharacterMovement()->AirControl = CharacterSettings->AirControl;
		GetCharacterMovement()->MaxWalkSpeed = CharacterSettings->DefaultWalkingSpeed;
		GetCharacterMovement()->MinAnalogWalkSpeed = CharacterSettings->MinAnalogWalkSpeed;
		GetCharacterMovement()->BrakingDecelerationWalking = CharacterSettings->BrakingDecelerationWalking;
		GetCharacterMovement()->BrakingDecelerationFalling = CharacterSettings->BrakingDecelerationFalling;
	}
}

void ACharacterBase::TriggerInteraction()
{
	UInteractionComponent* InteractionComponent = GetComponentByClass<UInteractionComponent>();
	if (!IsValid(InteractionComponent))
	{
		return;
	}
	EInteractionCategory InteractionCategory = InteractionComponent->GetCurrentInteractionType();
	if (InteractionCategory != EInteractionCategory::Undefined)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
		if (IsValid(SkeletalMeshComponent))
		{
			UAnimInstanceSimple* SimpleAnimInstance = Cast<UAnimInstanceSimple>(SkeletalMeshComponent->GetAnimInstance());
			if (IsValid(SimpleAnimInstance))
			{
				if (SimpleAnimInstance->bIsInteractionAnimPlaying )
				{
					return;
				}
				if (SimpleAnimInstance->TryPlayInteractAnimationMontage())
				{
					InteractionComponent->SnapActorToCurrentInteractable();
					return;
				}
			}
		}
	}
	//If getting the InteractionAnimation failed, trigger interaction immediately
	InteractionComponent->Interact();
}

void ACharacterBase::OnCommandSaveData_Implementation(USaveGameData* SaveGameFile)
{
	UE_LOG(LogSave, Display, TEXT(" %s : Save Data"), *GetFName().ToString())
}

void ACharacterBase::OnCommandLoadData_Implementation(USaveGameData* SaveGameFile)
{
	UE_LOG(LogSave, Display, TEXT(" %s : Load Data"), *GetFName().ToString())
}

void ACharacterBase::Move(const FInputActionValue& Input)
{
	if (!IsValid(Controller))
	{
		UE_LOG(LogCharacter, Error, TEXT("No Controller"));
		return;
	}
	const FVector2D& MovementVector = Input.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ACharacterBase::Look(const FInputActionValue& Input)
{
	const FVector2D& LookAxisVector = Input.Get<FVector2D>();

	// add yaw and pitch input to controller
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#include "CharacterBase.h"

#include "../Animation/AnimInstanceSimple.h"
#include "Components/AttributeComponent.h"
#include "Components/CombatComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/FootstepAudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectMirror/InteractionComponent.h"
#include "ProjectMirror/Interfaces/Ignitable.h"
#include "ProjectMirror/Interfaces/Interactable.h"
#include "ProjectMirror/SaveSystem/SubsystemSaveAndLoad.h"
#include "ProjectMirror/Settings/SettingsCharacter.h"
#include "ProjectMirror/Settings/SettingsCombatFeedback.h"
#include "ProjectMirror/Utility/CombatData.h"
#include "ProjectMirror/Utility/Utilities.h"

DEFINE_LOG_CATEGORY(LogCharacter);

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	FootstepAudioComponent = CreateDefaultSubobject<UFootstepAudioComponent>(TEXT("FootstepAudioComponent"));
	FootstepAudioComponent->SetupAttachment(GetMesh(), TEXT("Root"));

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

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

void ACharacterBase::ReceiveDamage(const FDamage& Damage, AActor* DamageInstigator)
{
	if (!IsValid(AttributeComponent))
	{
		return;
	}

	FDefenseStats TotalDefenseStats;
	if (IsValid(EquipmentComponent))
	{
		TotalDefenseStats = EquipmentComponent->GetTotalDefenseStats();
	}

	// PLACEHOLDER: purely subtractive per channel, clamped at zero, channels summed.                                                                                                                                                                                                                                                                                                                                        
	// The real mitigation curve is still an open design decision.                                                                                                                                                                                                                                                                                                                                                           
	const FAttackStats& AttackStats = Damage.AttackStats;
	FAttackStats MitigatedStats;
	MitigatedStats.AttackPowerPhysical = FMath::Max(0.0f, AttackStats.AttackPowerPhysical - TotalDefenseStats.DefensePhysical);
	MitigatedStats.AttackPowerMagic = FMath::Max(0.0f, AttackStats.AttackPowerMagic - TotalDefenseStats.DefenseMagic);
	MitigatedStats.AttackPowerLight = FMath::Max(0.0f, AttackStats.AttackPowerLight - TotalDefenseStats.DefenseLight);
	MitigatedStats.AttackPowerFire = FMath::Max(0.0f, AttackStats.AttackPowerFire - TotalDefenseStats.DefenseFire);
	MitigatedStats.AttackPowerDarkness = FMath::Max(0.0f, AttackStats.AttackPowerDarkness - TotalDefenseStats.DefenseDarkness);

	const float TotalDamage = MitigatedStats.AttackPowerPhysical + MitigatedStats.AttackPowerMagic
		+ MitigatedStats.AttackPowerLight + MitigatedStats.AttackPowerFire + MitigatedStats.AttackPowerDarkness;

	// Undefined means every channel was absorbed. That is a feedback case of its own,                                                                                                                                                                                                                                                                                                                                       
	// which is why the hit is reported before the damage check, not after it.                                                                                                                                                                                                                                                                                                                                               
	const EDamageChannel DominantChannel = ProjectMirrorUtilities::ResolveDominantDamageChannel(MitigatedStats);
	PlayTargetHitFeedback(DominantChannel, Damage.ImpactLocation, Damage.ImpactNormal);

	if (TotalDamage <= 0.0f)
	{
		return;
	}

	AttributeComponent->LoseHealth(TotalDamage);
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
	CacheTargetFeedback();
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
				if (SimpleAnimInstance->bIsInteractionAnimPlaying)
				{
					return;
				}

				InteractionComponent->CacheCurrentInteractable();
				EEquipmentSlot TargetEquipmentSlot = EEquipmentSlot::Undefined;
				const bool bDoesInteractionAquireItem = InteractionCategory == EInteractionCategory::Take || InteractionCategory == EInteractionCategory::Open;
				if (bDoesInteractionAquireItem)
				{
					TargetEquipmentSlot = CheckForRelevantSlot(InteractionComponent);
					EquipmentComponent->HideActiveWeaponsForInteraction(InteractionCategory, TargetEquipmentSlot);
				}
				else if (InteractionCategory == EInteractionCategory::Ignite)
				{
					const IIgnitable* IgnitableTarget = Cast<IIgnitable>(InteractionComponent->GetCurrentInteractable());
					const bool bWantIgnitedWeapon = IgnitableTarget ? !IgnitableTarget->IsIgnited() : false;
					TargetEquipmentSlot = EquipmentComponent->ResolveIgnitableSlot(bWantIgnitedWeapon);
				}
				if (SimpleAnimInstance->TryPlayInteractAnimationMontage(InteractionCategory, InteractionComponent->ResolveInteractionHeight(),
				                                                        TargetEquipmentSlot))
				{
					InteractionComponent->SnapActorToCurrentInteractable();
					return;
				}
			}
		}
	}
	//If getting the InteractionAnimation failed, trigger interaction immediately
	InteractionComponent->CacheCurrentInteractable();
	InteractionComponent->Interact();
	EquipmentComponent->CommitPickupVisuals();
}

EEquipmentSlot ACharacterBase::CheckForRelevantSlot(const UInteractionComponent* InteractionComponent)
{
	if (IsValid(EquipmentComponent) && IsValid(InteractionComponent))
	{
		if (const IInteractable* Interactable = Cast<IInteractable>(InteractionComponent->GetCurrentInteractable()))
		{
			if (const UDefinitionEquipment* DefinitionEquipment = Cast<UDefinitionEquipment>(Interactable->GetContainedItemDefinition()))
			{
				return EquipmentComponent->ResolveEquipmentSlot(DefinitionEquipment);
			}
		}
	}
	return EEquipmentSlot::Undefined;
}

void ACharacterBase::OnCommandSaveData_Implementation(USaveGameData* SaveGameFile)
{
	UE_LOG(LogSave, Display, TEXT(" %s : Save Data"), *GetFName().ToString())
}

void ACharacterBase::OnCommandLoadData_Implementation(USaveGameData* SaveGameFile)
{
	UE_LOG(LogSave, Display, TEXT(" %s : Load Data"), *GetFName().ToString())
}

void ACharacterBase::PlayTargetHitFeedback(const EDamageChannel DamageChannel, const FVector& ImpactLocation, const FVector& ImpactNormal) const
{
	ProjectMirrorUtilities::PlayHitFeedbackForKey(GetWorld(), CachedTargetFeedback, DamageChannel, CachedDefaultTargetFeedback, ImpactLocation, ImpactNormal);
}

void ACharacterBase::CacheTargetFeedback()
{
	const USettingsCombatFeedback* CombatFeedbackSettings = GetDefault<USettingsCombatFeedback>();
	if (!CombatFeedbackSettings)
	{
		return;
	}
	
	CachedDefaultTargetFeedback = ProjectMirrorUtilities::ResolveHitFeedbackEntry(CombatFeedbackSettings->DefaultTargetFeedback);

	CachedTargetFeedback.Reset();

	const FHitFeedbackChannelSet* TargetChannelSet = CombatFeedbackSettings->TargetFeedbackMap.Find(CreatureType);
	if (!TargetChannelSet)
	{
		return;
	}

	ProjectMirrorUtilities::ResolveHitFeedbackMap(TargetChannelSet->ChannelFeedbackMap, CachedTargetFeedback);
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

// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "AnimInstanceSimple.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectMirror/InteractionComponent.h"
#include "ProjectMirror/Characters/CharacterBase.h"
#include "ProjectMirror/Characters/Components/ClimbComponent.h"
#include "ProjectMirror/Characters/Components/EquipmentComponent.h"
#include "ProjectMirror/UI/HUDInGame.h"

static constexpr const TCHAR* PutOnPrimaryBackupSection = TEXT("PutOnHip");
static constexpr const TCHAR* PutOnSecondaryBackupSection = TEXT("PutOnHip_L");
static constexpr const TCHAR* PutOnTwoHandedBackupSection = TEXT("PutOnBack");
static constexpr const TCHAR* EndSection = TEXT("End");

void UAnimInstanceSimple::SetSustainedAction(bool bIsSustaining, ESustainedPose Pose)
{
	bIsSustainingAction = bIsSustaining;

	if (bIsSustaining)
	{
		CurrentSustainedPose = Pose;
	}
}

void UAnimInstanceSimple::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<ACharacterBase>(GetOwningActor());
	if (!IsValid(Character))
	{
		return;
	}
	MovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
}

void UAnimInstanceSimple::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!IsValid(MovementComponent))
	{
		return;
	}
	Velocity = MovementComponent->Velocity;

	const FVector& HorizontalMovementSpeed = FVector{Velocity.X, Velocity.Y, 0.0f};
	GroundSpeed = HorizontalMovementSpeed.Length();
	FacingDirection = UKismetAnimationLibrary::CalculateDirection(HorizontalMovementSpeed, Character->GetActorRotation());

	bShouldMove = GroundSpeed > 3.0f;
	bIsFalling = MovementComponent->IsFalling();
}

bool UAnimInstanceSimple::TryPlayInteractAnimationMontage(const EInteractionCategory InteractionCategory, const EInteractionHeight InteractionHeight,
                                                          const EEquipmentSlot TargetSlot)
{
	UAnimMontage* DesiredMontage = nullptr;
	bool bIsPickupMontage = false;
	switch (InteractionCategory)
	{
	case EInteractionCategory::Interact:
		DesiredMontage = InteractionAnimMontage;
		break;
	case EInteractionCategory::Take:
		DesiredMontage = GetPickupMontageHeightVariant(InteractionCategory, InteractionHeight);
		bIsPickupMontage = true;
		break;
	case EInteractionCategory::Open:
		DesiredMontage = GetPickupMontageHeightVariant(InteractionCategory, InteractionHeight);
		bIsPickupMontage = true;
		break;
	case EInteractionCategory::Push:
		DesiredMontage = PushAnimMontage;
		break;
	case EInteractionCategory::Ignite:
		DesiredMontage = IgniteAnimMontage;
		bIsPickupMontage = true;
		break;
	default:
		return false;
	}

	if (!IsValid(DesiredMontage))
	{
		return false;
	}

	if (bIsPickupMontage)
	{
		TryConfigPickUpMontageAndPlay(DesiredMontage, TargetSlot);
	}
	else
	{
		if (Montage_Play(DesiredMontage) > 0.0f)
		{
			bIsInteractionAnimPlaying = true;
		}
	}

	if (bIsInteractionAnimPlaying)
	{
		FOnMontageBlendingOutStarted AnimEndDelegate;
		AnimEndDelegate.BindUObject(this, &UAnimInstanceSimple::HandleOnInteractAnimationFinished);
		Montage_SetBlendingOutDelegate(AnimEndDelegate, DesiredMontage);
		return true;
	}
	return false;
}

void UAnimInstanceSimple::TryPlayRestingAnimationMontage()
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}

	AHUDInGame* InGameHUD = Cast<AHUDInGame>(PlayerController->GetHUD());
	if (!IsValid(InGameHUD))
	{
		return;
	}

	FAlphaBlendArgs BlendSettings;
	BlendSettings.BlendTime = SitMontageBlendTime;
	if (!IsValid(RestingAnimMontage) || Montage_PlayWithBlendIn(RestingAnimMontage, BlendSettings) <= 0.0)
	{
		InGameHUD->PushInGameMenu();
	}

	InGameHUD->OnInGameMenuClosed.AddUObject(this, &UAnimInstanceSimple::HandlePauseMenuClosed);
}

bool UAnimInstanceSimple::TryPlayClimbAnimation()
{
	if (!IsValid(ClimbAnimMontage) || Montage_Play(ClimbAnimMontage) <= 0.0f)
	{
		return false;
	}
	return true;
}

bool UAnimInstanceSimple::TryPullFromLedgeMontage()
{
	if (!IsValid(PullFromLedgeMontage) || Montage_Play(PullFromLedgeMontage) <= 0.0f)
	{
		return false;
	}

	FOnMontageBlendingOutStarted AnimEndDelegate;
	AnimEndDelegate.BindUObject(this, &UAnimInstanceSimple::HandleOnPullUpFromLedgeAnimationFinished);
	Montage_SetBlendingOutDelegate(AnimEndDelegate, PullFromLedgeMontage);
	return true;
}

void UAnimInstanceSimple::HandlePauseMenuClosed()
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}
	AHUDInGame* InGameHUD = Cast<AHUDInGame>(PlayerController->GetHUD());
	if (!IsValid(InGameHUD))
	{
		return;
	}
	InGameHUD->OnInGameMenuClosed.RemoveAll(this);

	if (!IsValid(RestingAnimMontage))
	{
		return;
	}
	Montage_Stop(SitMontageBlendTime, RestingAnimMontage);
}

void UAnimInstanceSimple::HandleOnInteractAnimationFinished(UAnimMontage* Montage, bool bInterrupted)
{
	bIsInteractionAnimPlaying = false;
	bMirrorCurrentMontage = false;

	const AActor* Owner = GetOwningActor();
	if (!IsValid(Owner))
	{
		return;
	}
	if (UEquipmentComponent* EquipmentComponent = Owner->FindComponentByClass<UEquipmentComponent>())
	{
		EquipmentComponent->CommitPickupVisuals();
	}
	UInteractionComponent* InteractionComponent = Owner->GetComponentByClass<UInteractionComponent>();
	{
		if (IsValid(InteractionComponent))
		{
			InteractionComponent->EmptyInteractableCache();
		}
	}
}

void UAnimInstanceSimple::HandleOnRestingAnimationFinished(UAnimMontage* Montage, bool bInterrupted)
{
	HandlePauseMenuClosed();
}

void UAnimInstanceSimple::HandleOnPullUpFromLedgeAnimationFinished(UAnimMontage* Montage, bool bInterrupted)
{
	if (!IsValid(PullFromLedgeMontage) || !bInterrupted)
	{
		return;
	}
	AActor* Owner = GetOwningActor();
	if (IsValid(Owner))
	{
		UClimbComponent* ClimbComponent = Owner->FindComponentByClass<UClimbComponent>();
		if (IsValid(ClimbComponent))
		{
			ClimbComponent->SnapOnLedgeSurface();
		}
	}
}

void UAnimInstanceSimple::HandleClimbStarted()
{
	CurrentActionState = ECharacterAnimationState::Hanging;
}

void UAnimInstanceSimple::HandleClimbEnd()
{
	CurrentActionState = ECharacterAnimationState::Idle;
}

void UAnimInstanceSimple::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	AActor* Owner = GetOwningActor();
	if (IsValid(Owner))
	{
		UEquipmentComponent* EquipmentComponent = Owner->FindComponentByClass<UEquipmentComponent>();
		if (IsValid(EquipmentComponent))
		{
			EquipmentComponent->OnWeaponChanged.AddUniqueDynamic(this, &UAnimInstanceSimple::HandleWeaponChange);
		}

		UClimbComponent* ClimbComponent = Owner->FindComponentByClass<UClimbComponent>();
		if (IsValid(ClimbComponent))
		{
			ClimbComponent->OnClimbStart.AddUObject(this, &UAnimInstanceSimple::HandleClimbStarted);
			ClimbComponent->OnClimbEnd.AddUObject(this, &UAnimInstanceSimple::HandleClimbEnd);
		}
	}
}

void UAnimInstanceSimple::HandleWeaponChange(EWeaponType NewWeaponType, bool bIsOffhand)
{
	if (bIsOffhand)
	{
		LeftHandWeapon = NewWeaponType;
	}
	else
	{
		RightHandWeapon = NewWeaponType;
	}
}

UAnimMontage* UAnimInstanceSimple::GetPickupMontageHeightVariant(const EInteractionCategory PickUpCategory, const EInteractionHeight PickUpHeight)
{
	if (PickUpCategory == EInteractionCategory::Take)
	{
		switch (PickUpHeight)
		{
		case EInteractionHeight::Low:
			return TakeAnimMontageLow;
		case EInteractionHeight::Mid:
			return TakeAnimMontageMid;
		case EInteractionHeight::High:
			return TakeAnimMontageHigh;
		default:
			return TakeAnimMontageMid;
		}
	}
	if (PickUpCategory == EInteractionCategory::Open)
	{
		switch (PickUpHeight)
		{
		case EInteractionHeight::Low:
			return OpenAnimMontageLow;
		case EInteractionHeight::Mid:
			return OpenAnimMontageMid;
		case EInteractionHeight::High:
			return OpenAnimMontageMid;
		default:
			return OpenAnimMontageLow;
		}
	}
	return nullptr;
}

bool UAnimInstanceSimple::TryConfigPickUpMontageAndPlay(UAnimMontage* PickUpMontage, const EEquipmentSlot TargetSlot)
{
	if (!IsValid(PickUpMontage))
	{
		return false;
	}
	FName FollowupSection = NAME_None;
	bool bMirrorAnimation = false;

	switch (TargetSlot)
	{
	case EEquipmentSlot::MainHand:
		FollowupSection = EndSection;
		break;
	case EEquipmentSlot::OffHand:
		FollowupSection = EndSection;
		bMirrorAnimation = true;
		break;
	case EEquipmentSlot::TwoHanded:
		FollowupSection = EndSection;
		break;
	case EEquipmentSlot::BackupPrimary:
		FollowupSection = PutOnPrimaryBackupSection;
		break;
	case EEquipmentSlot::BackupSecondary:
		FollowupSection = PutOnSecondaryBackupSection;
		bMirrorAnimation = true;
		break;
	case EEquipmentSlot::BackupTwoHanded:
		FollowupSection = PutOnTwoHandedBackupSection;
		break;
	default:
		FollowupSection = EndSection;
		break;
	}

	if (Montage_Play(PickUpMontage) > 0.0f)
	{
		//Play with FollowUpSection
		bIsInteractionAnimPlaying = true;
		bMirrorCurrentMontage = bMirrorAnimation;

		Montage_SetNextSection(Montage_GetCurrentSection(PickUpMontage), FollowupSection);
		return true;
	}
	return false;
}

bool UAnimInstanceSimple::TryPerformAttackAnimation(UAnimMontage* WeaponMontage, const FName& SectionName)
{
	if (IsValid(WeaponMontage))
	{
		if (Montage_Play(WeaponMontage) > 0.0f)
		{
			if (SectionName != NAME_None)
			{
				Montage_JumpToSection(SectionName);
			}
			return true;
		}
	}
	return false;
}

bool UAnimInstanceSimple::IsAttackMontagePlaying(const UAnimMontage* WeaponMontage) const
{
	return IsValid(WeaponMontage) && Montage_IsPlaying(WeaponMontage);
}

bool UAnimInstanceSimple::TryChainAttackAnimation(UAnimMontage* WeaponMontage, const FName& SectionName)
{
	if (!IsValid(WeaponMontage) || SectionName == NAME_None)
	{
		return false;
	}

	const FName CurrentSection = Montage_GetCurrentSection(WeaponMontage);
	if (CurrentSection == NAME_None)
	{
		return false;
	}

	Montage_SetNextSection(CurrentSection, SectionName, WeaponMontage);
	return true;
}

void UAnimInstanceSimple::SetSustainedAction(bool bIsSustaining, ESustainedPose Pose, EEquipmentSlot Slot)
{
	bIsSustainingAction = bIsSustaining;

	// Pose und Slot bleiben beim Beenden stehen: die State Machine blendet danach noch                                                                                                                                                                                                                                                                                                                                      
	// aus dem Sustained-State heraus und braucht bis zum Ende dieselbe Maske.                                                                                                                                                                                                                                                                                                                                               
	if (bIsSustaining)
	{
		CurrentSustainedPose = Pose;
		SustainedSlot = Slot;
	}
}

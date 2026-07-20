// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "CombatComponent.h"

#include "EquipmentComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectMirror/Animation/AnimInstanceSimple.h"
#include "ProjectMirror/Interfaces/Damageable.h"
#include "ProjectMirror/Objects/Equipment/Weapons/WeaponStaticBase.h"
#include "ProjectMirror/Settings/SettingsCombatFeedback.h"
#include "ProjectMirror/Utility/Utilities.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	const AActor* OwningActor = GetOwner();
	if (!IsValid(OwningActor))
	{
		return;
	}

	CachedEquipmentComponent = OwningActor->FindComponentByClass<UEquipmentComponent>();

	const USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(OwningActor->FindComponentByClass<USkeletalMeshComponent>());
	if (!IsValid(SkeletalMeshComponent))
	{
		return;
	}

	CachedAnimInstance = Cast<UAnimInstanceSimple>(SkeletalMeshComponent->GetAnimInstance());
	if (IsValid(CachedAnimInstance))
	{
		CachedAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UCombatComponent::HandleMontageBlendingOut);
	}

	CacheSurfaceFeedback();
}

void UCombatComponent::HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	// Safety net for the case NotifyEnd never runs: a montage cut short by a stagger,
	// a death or a level change would otherwise leave the hitbox open for good.
	// EndAttackWindow is idempotent, so the redundant call on a normal finish costs nothing.
	EndAttackWindow();

	if (Montage != ActiveAttackMontage)
	{
		return;
	}

	AttackCount = 0;
	bIsComboWindowOpen = false;
	ActiveAttackMontage = nullptr;
	ActiveAttackInput = FCombatInputIdentity();

	// Only after a chain ran to its end. On an interrupt something else took over —                                                                                                                                                                                                                                                                                                                                         
	// raising the guard into a stagger or death animation would be wrong.                                                                                                                                                                                                                                                                                                                                                   
	if (!bInterrupted)
	{
		ReevaluateHeldInputs();
	}
}

void UCombatComponent::CacheSurfaceFeedback()
{
	const USettingsCombatFeedback* FeedbackSettings = GetDefault<USettingsCombatFeedback>();
	if (!FeedbackSettings)
	{
		return;
	}

	ProjectMirrorUtilities::ResolveHitFeedbackMap(FeedbackSettings->SurfaceFeedbackMap, CachedSurfaceFeedbackMap);
	CachedDefaultSurfaceFeedback = ProjectMirrorUtilities::ResolveHitFeedbackEntry(FeedbackSettings->DefaultSurfaceFeedback);
}

bool UCombatComponent::IsPerformingAttack() const

{
	return IsValid(ActiveAttackMontage) && IsValid(CachedAnimInstance) && CachedAnimInstance->IsAttackMontagePlaying(ActiveAttackMontage);
}


void UCombatComponent::BeginAttackWindow(EEquipmentSlot AttackingSlot)
{
	// A previous window that never closed would otherwise keep its hitbox open forever.
	EndAttackWindow();

	ActorsHitThisSwing.Empty();

	AWeaponStaticBase* AttackingWeapon = ResolveAttackingWeapon(AttackingSlot);
	if (!IsValid(AttackingWeapon))
	{
		return;
	}

	if (const UCapsuleComponent* Hitbox = AttackingWeapon->GetAttackHitbox())
	{
		PreviousTraceLocation = Hitbox->GetComponentLocation();
	}

	ActiveAttackWeapon = AttackingWeapon;
	AttackingWeapon->StartAttackFeedback();
}

void UCombatComponent::EndAttackWindow()
{
	ActiveAttackWeapon = nullptr;
}

void UCombatComponent::PlayEnvironmentHitFeedback(const FHitResult& Hit)
{
	if (Hit.bStartPenetrating)
	{
		return;
	}

	TEnumAsByte<EPhysicalSurface> SurfaceType = SurfaceType_Default;
	if (const UPhysicalMaterial* SurfaceMaterial = Hit.PhysMaterial.Get())
	{
		SurfaceType = SurfaceMaterial->SurfaceType;
	}

	ProjectMirrorUtilities::PlayHitFeedbackForKey(GetWorld(), CachedSurfaceFeedbackMap, SurfaceType, CachedDefaultSurfaceFeedback, Hit.ImpactPoint,
	                                              Hit.ImpactNormal);
}

void UCombatComponent::HandleWeaponHit(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();

	if (!IsValid(ActiveAttackWeapon) || !IsValid(HitActor))
	{
		return;
	}

	if (ActorsHitThisSwing.Contains(HitActor))
	{
		return;
	}
	ActorsHitThisSwing.Add(HitActor);

	IDamageable* DamageableTarget = Cast<IDamageable>(HitActor);
	if (!DamageableTarget)
	{
		PlayEnvironmentHitFeedback(Hit);
		return;
	}

	FDamage Damage;
	Damage.AttackStats = ActiveAttackWeapon->GetAttackStats();
	Damage.ImpactLocation = Hit.ImpactPoint;
	Damage.ImpactNormal = Hit.ImpactNormal;

	if (Hit.bStartPenetrating)
	{
		Damage.ImpactNormal = (GetOwner()->GetActorLocation() - Hit.ImpactPoint).GetSafeNormal();
	}

	DamageableTarget->ReceiveDamage(Damage, GetOwner());
}

void UCombatComponent::PerformWeaponAction(const FCombatInputIdentity& CombatInputIdentity)
{
	HeldCombatInputs.Remove(CombatInputIdentity);
	HeldCombatInputs.Add(CombatInputIdentity);

	if (!IsValid(CachedAnimInstance) || !IsValid(CachedEquipmentComponent))
	{
		return;
	}

	const FWeaponAction Action = ResolveWeaponAction(CombatInputIdentity);

	if (IsPerformingAttack())
	{
		if (!bIsComboWindowOpen)
		{
			return;
		}

		if (CombatInputIdentity != ActiveAttackInput)
		{
			return;
		}

		const int32 NextAttackIndex = AttackCount + 1;
		if (!Action.ComboSections.IsValidIndex(NextAttackIndex))
		{
			return;
		}
		if (CachedAnimInstance->TryChainAttackAnimation(Action.Montage, Action.ComboSections[NextAttackIndex]))
		{
			AttackCount = NextAttackIndex;
			bIsComboWindowOpen = false;
		}
		return;
	}

	if (Action.Mode == EWeaponActionMode::Sustained)
	{
		BeginSustainedAction(Action, CombatInputIdentity);
		return;
	}

	if (!IsValid(Action.Montage))
	{
		return;
	}

	StartAttackChain(Action, CombatInputIdentity);
}

void UCombatComponent::ReevaluateHeldInputs()
{
	// Walk back to front: among the still-held inputs the most recent one wins, but only                                                                                                                                                                                                                                                                                                                                    
	// a sustained action may resume here. An instant action must never restart on its own —                                                                                                                                                                                                                                                                                                                                 
	// that would be the per-frame auto-attack the old project had.                                                                                                                                                                                                                                                                                                                                                          
	for (int32 Index = HeldCombatInputs.Num() - 1; Index >= 0; --Index)
	{
		const FCombatInputIdentity& HeldInput = HeldCombatInputs[Index];
		const FWeaponAction Action = ResolveWeaponAction(HeldInput);

		if (Action.Mode == EWeaponActionMode::Sustained)
		{
			BeginSustainedAction(Action, HeldInput);
			return;
		}
	}
}

void UCombatComponent::BeginSustainedAction(const FWeaponAction& Action, const FCombatInputIdentity& InputIdentity)
{
	if (ActiveSustainedInput == InputIdentity)
	{
		return;
	}
	EndSustainedAction();

	ActiveSustainedAction = Action;
	ActiveSustainedInput = InputIdentity;

	if (IsValid(CachedAnimInstance))
	{
		CachedAnimInstance->SetSustainedAction(true, Action.SustainedPose, InputIdentity.Slot);
	}
}

void UCombatComponent::EndSustainedAction()
{
	if (!HasActiveSustainedAction())
	{
		return;
	}

	ActiveSustainedAction = FWeaponAction();
	ActiveSustainedInput = FCombatInputIdentity();

	if (IsValid(CachedAnimInstance))
	{
		CachedAnimInstance->SetSustainedAction(false, ESustainedPose::Guard);
	}
}

void UCombatComponent::TickAttackTrace()
{
	if (!IsValid(ActiveAttackWeapon))
	{
		return;
	}

	const UCapsuleComponent* Hitbox = ActiveAttackWeapon->GetAttackHitbox();
	if (!IsValid(Hitbox))
	{
		return;
	}

	const FVector CurrentLocation = Hitbox->GetComponentLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());

	TArray<AActor*> AttachedActors;
	GetOwner()->GetAttachedActors(AttachedActors);
	QueryParams.AddIgnoredActors(AttachedActors);

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(Hits, PreviousTraceLocation, CurrentLocation, Hitbox->GetComponentQuat(), ECC_GameTraceChannel3,
	                                FCollisionShape::MakeCapsule(Hitbox->GetScaledCapsuleRadius(), Hitbox->GetScaledCapsuleHalfHeight()), QueryParams);

	for (const FHitResult& Hit : Hits)
	{
		HandleWeaponHit(Hit);
	}

	PreviousTraceLocation = CurrentLocation;
}

void UCombatComponent::ReleaseWeaponAction(const FCombatInputIdentity& CombatInput)
{
	HeldCombatInputs.Remove(CombatInput);

	if (ActiveSustainedInput == CombatInput)
	{
		EndSustainedAction();
	}
}

void UCombatComponent::CancelCurrentWeaponAction()
{
	EndSustainedAction();
}

AWeaponStaticBase* UCombatComponent::ResolveAttackingWeapon(EEquipmentSlot HandSlot) const
{
	if (!IsValid(CachedEquipmentComponent))
	{
		return nullptr;
	}

	AEquipmentBase* EquipmentActor = CachedEquipmentComponent->GetActiveWeaponActor(HandSlot);

	if (!IsValid(EquipmentActor) && HandSlot == EEquipmentSlot::MainHand)
	{
		EquipmentActor = CachedEquipmentComponent->GetActiveWeaponActor(EEquipmentSlot::TwoHanded);
	}

	return Cast<AWeaponStaticBase>(EquipmentActor);
}

FWeaponAction UCombatComponent::ResolveWeaponAction(const FCombatInputIdentity& CombatInputIdentity) const
{
	if (!IsValid(CachedEquipmentComponent))
	{
		return FWeaponAction();
	}

	UDefinitionEquipment* EquipmentDefinition = CachedEquipmentComponent->GetEquipmentDefinition(CombatInputIdentity.Slot);

	// A two-handed weapon does not sit in MainHand, so a MainHand attack has to look there as well.
	// Kept identical to ResolveAttackingWeapon: montage and hitbox must resolve to the same weapon.
	if (!IsValid(EquipmentDefinition) && CombatInputIdentity.Slot == EEquipmentSlot::MainHand)
	{
		EquipmentDefinition = CachedEquipmentComponent->GetEquipmentDefinition(EEquipmentSlot::TwoHanded);
	}

	if (!IsValid(EquipmentDefinition))
	{
		return FWeaponAction();
	}

	return CombatInputIdentity.bIsHeavyVariant ? EquipmentDefinition->GetHeavyAction() : EquipmentDefinition->GetNormalAction();
}

FName UCombatComponent::ResolveMontageSectionName(const FWeaponAction& AttackChain) const
{
	return AttackChain.ComboSections.IsValidIndex(AttackCount) ? AttackChain.ComboSections[AttackCount] : NAME_None;
}

void UCombatComponent::StartAttackChain(const FWeaponAction& Action, const FCombatInputIdentity& InputIdentity)
{
	// An instant action always drops a held one for now; step 7 makes this conditional.                                                                                                                                                                                                                                                                                                                                     
	EndSustainedAction();

	AttackCount = 0;
	bIsComboWindowOpen = false;
	ActiveAttackMontage = Action.Montage;
	ActiveAttackInput = InputIdentity;

	CachedAnimInstance->TryPerformAttackAnimation(Action.Montage, ResolveMontageSectionName(Action));
}

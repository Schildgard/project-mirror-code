// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "ProjectMirror/Interfaces/Damageable.h"
#include "ProjectMirror/Interfaces/Saveable.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "ProjectMirror/Utility/HitFeedbackData.h"
#include "CharacterBase.generated.h"


class UAttributeComponent;
class UCombatComponent;
class UInteractionComponent;
class UEquipmentComponent;
class UFootstepAudioComponent;
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

UCLASS()
class PROJECTMIRROR_API ACharacterBase : public ACharacter, public ISaveable, public IDamageable
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	FName CharacterID = NAME_None;
	UPROPERTY(VisibleAnywhere)
	ECreatureType CreatureType = ECreatureType::Humanoid_Normal;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UFootstepAudioComponent> FootstepAudioComponent;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> CombatComponent;

	UPROPERTY()
	TMap<EDamageChannel, FResolvedHitFeedback> CachedTargetFeedback;

	UPROPERTY()
	FResolvedHitFeedback CachedDefaultTargetFeedback;

	void CacheTargetFeedback();
	void PlayTargetHitFeedback(const EDamageChannel DamageChannel, const FVector& ImpactLocation, const FVector& ImpactNormal) const;

public:
	void Move(const FInputActionValue& Input);
	void Look(const FInputActionValue& Input);

	void SetStrafing(const bool IsStrafing) const;;
	void SetupMovementSettings();
	void TriggerInteraction();
	UAttributeComponent* GetAttributeComponent() const { return AttributeComponent; }
	EEquipmentSlot CheckForRelevantSlot(const UInteractionComponent* InteractionComponent);

	ECreatureType GetCreatureType() const { return CreatureType; }

	virtual void ReceiveDamage(const FDamage& Damage, AActor* DamageInstigator) override;

	virtual void OnCommandSaveData_Implementation(USaveGameData* SaveGameFile) override;
	virtual void OnCommandLoadData_Implementation(USaveGameData* SaveGameFile) override;

};

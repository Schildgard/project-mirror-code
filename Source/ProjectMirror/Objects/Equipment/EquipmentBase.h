// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectMirror/Data/DefinitionEquipment.h"
#include "ProjectMirror/Utility/CombatData.h"
#include "EquipmentBase.generated.h"

class UEffectComponentBase;
class UWeaponEffect;
struct FSavedEffectData;
enum class EEquipmentSlot : uint8;
struct FEquipmentBundle;
class UDefinitionEquipment;

UCLASS(Abstract)
class PROJECTMIRROR_API AEquipmentBase : public AActor
{
	GENERATED_BODY()

	protected:
	AEquipmentBase();
	
	UPROPERTY()
	TObjectPtr<UDefinitionEquipment> SourceDefinition;

public:

	virtual void AttachToOwner(USkeletalMeshComponent* OwnerMesh, const FName& Socket);
	void BeginDrop(const FEquipmentBundle& DataBundle);
	virtual UMeshComponent* GetMesh() const { return nullptr; }
	virtual void OnSlotChanged(const EEquipmentSlot NewSlot);
	TArray<FSavedEffectData> GetEquipmentEffectsForSave() const;
	void ApplyEffectsFromSave(const TArray<FSavedEffectData>& SavedEffects);
	UEffectComponentBase* FindEffectOfClass(TSubclassOf<UEffectComponentBase> EffectClass) const;
	void ApplyInitialEffectStates(const TMap<TSubclassOf<UEffectComponentBase>, bool>& States);

	FAttackStats GetAttackStats() const { return SourceDefinition ? SourceDefinition->GetAttackStats() : FAttackStats(); }
	FDefenseStats GetDefenseStats() const { return SourceDefinition ? SourceDefinition->GetDefenseStats() : FDefenseStats(); }
	FBlockStats GetBlockStats() const { return SourceDefinition ? SourceDefinition->GetBlockStats() : FBlockStats(); }
	
	void SetSourceDefinition(UDefinitionEquipment* Definition) { SourceDefinition = Definition; }
};

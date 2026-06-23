// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "ProjectMirror/Interfaces/Saveable.h"
#include "ProjectMirror/Utility/EnumCollection.h"
#include "CharacterBase.generated.h"

class UFootstepAudioComponent;
DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

UCLASS()
class PROJECTMIRROR_API ACharacterBase : public ACharacter, public ISaveable
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

public:
	void Move(const FInputActionValue& Input);
	void Look(const FInputActionValue& Input);
	
	void Run();
	void CancelRunning();
	void SetStrafing(const bool IsStrafing)const;;
	void SetupMovementSettings();
	void TriggerInteraction();
	
	ECreatureType GetCreatureType() const {return CreatureType;}
	
	virtual void OnCommandSaveData_Implementation(USaveGameData* SaveGameFile) override;
	virtual void OnCommandLoadData_Implementation(USaveGameData* SaveGameFile) override;
};

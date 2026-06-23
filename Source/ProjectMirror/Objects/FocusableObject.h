// Copyright 2026 Leonard Kemenani. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectMirror/Interfaces/Focusable.h"
#include "FocusableObject.generated.h"

UCLASS()
class PROJECTMIRROR_API AFocusableObject : public AActor, public IFocusable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName FocusEventID = NAME_None;
	UPROPERTY(EditAnywhere)
	FName FocusLostEventID = NAME_None;

public:
	AFocusableObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnFocused_Implementation() override;
	virtual void OnFocusLost_Implementation() override;
};

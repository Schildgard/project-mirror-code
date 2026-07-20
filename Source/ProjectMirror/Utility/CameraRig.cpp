// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "CameraRig.h"


ACameraRig::ACameraRig()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraRigArm"));
	SetRootComponent(CameraArm);
	CameraArm->bDoCollisionTest = true;
	CameraArm->bUsePawnControlRotation = false;
	CameraArm->bEnableCameraLag = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("RigCamera"));
	Camera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
}

void ACameraRig::BeginPlay()
{
	Super::BeginPlay();
}

void ACameraRig::ApplyPose(const FVector& CamPivotOffset, const FRotator& ArmRotation, float ArmLength)
{
	CameraArm->SetRelativeLocation(CamPivotOffset);                                                                                                                                                                                                                                                                                                                                                                             
	CameraArm->SetRelativeRotation(ArmRotation);                                                                                                                                                                                                                                                                                                                                                                             
	CameraArm->TargetArmLength = ArmLength;
}

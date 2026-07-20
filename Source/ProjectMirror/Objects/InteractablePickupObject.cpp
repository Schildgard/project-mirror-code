// Copyright 2026 Leonard Kemenani. All Rights Reserved.


#include "InteractablePickupObject.h"


#include "Equipment/EquipmentBase.h"
#include "GameFramework/Character.h"
#include "ProjectMirror/Characters/Components/EquipmentComponent.h"
#include "ProjectMirror/Data/DefinitionEquipment.h"
#include "ProjectMirror/Data/DefinitionItem.h"
#include "ProjectMirror/GameWorld/SubsystemObjectEvents.h"
#include "ProjectMirror/Settings/SettingsObjects.h"


AInteractablePickupObject::AInteractablePickupObject()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionCategory = EInteractionCategory::Take;
}

void AInteractablePickupObject::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);

	if (bIsStowProxy)
	{
		// Spawned only as a transient hand proxy for an Open interaction: no world-pickup behaviour.
		return;
	}

	if (bIsFalling)
	{
		DropFromPlayer();
		return;
	}
	AdjustObjectTransform();
	if (!bIsInteractionPossible)
	{
		Destroy();
	}
}

void AInteractablePickupObject::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bIsStowBlending) { return; }

	StowBlendElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(StowBlendElapsed / StowBlendDuration, 0.f, 1.f);

	FTransform Current;
	Current.Blend(StowBlendStartRelative, FTransform::Identity, Alpha);
	StaticMeshComponent->SetRelativeTransform(Current);

	if (Alpha >= 1.f) { FinishStowBlend(); }
}

void AInteractablePickupObject::OnInteract_Implementation(UInteractionComponent* InteractingComponent)
{
	if (!IsValid(ContainedItemDefinition))
	{
		return;
	}

	if (!IsValid(InteractingComponent))
	{
		return;
	}

	const ACharacter* InteractingCharacter = Cast<ACharacter>(InteractingComponent->GetOwner());
	if (!IsValid(InteractingCharacter))
	{
		return;
	}

	USkeletalMeshComponent* InteractingActorMesh = InteractingCharacter->GetMesh();
	if (!IsValid(InteractingActorMesh))
	{
		return;
	}

	UDefinitionEquipment* EquipmentDefinition = Cast<UDefinitionEquipment>(ContainedItemDefinition);
	if (IsValid(EquipmentDefinition))
	{
		//@todo: For now it is okay to make this look for an Equipment Component since the only pickable objects are Equipments. Change this when there is an actual InventoryComponent and non-equipment pick ups
		UEquipmentComponent* EquipmentComponent = InteractingCharacter->GetComponentByClass<UEquipmentComponent>();
		if (!IsValid(EquipmentComponent))
		{
			return;
		}
		// Take: this pickup's own mesh is the item being grabbed and serves as the hand proxy.
		// Open: the item has no world pickup to grab, so spawn its world-pickup actor as a transient hand proxy.
		AInteractablePickupObject* HandProxy = InteractionCategory == EInteractionCategory::Open ? SpawnHandProxy(EquipmentDefinition) : this;
		const bool bHasHandProxy = IsValid(HandProxy);

		// Only defer visibility to the stow choreography when a proxy exists to carry it; otherwise equip visible.
		const EEquipmentSlot ResolvedSlot = EquipmentComponent->EquipAndReturnResolvedSlot(EquipmentDefinition, EventID, bHasHandProxy);
		
		if (AEquipmentBase* SpawnedEquip = EquipmentComponent->GetActiveWeaponActor(ResolvedSlot))
		{
			SpawnedEquip->ApplyInitialEffectStates(InitialEffectStates);
		}

		if (bHasHandProxy)
		{
			// Proxy mesh snaps into the grab hand socket; the real (invisible) weapon already sits in its slot.
			const FName TargetSocket = EquipmentComponent->ResolveGrabHandSocket(ResolvedSlot);

			HandProxy->StaticMeshComponent->SetSimulatePhysics(false);
			HandProxy->StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			HandProxy->StaticMeshComponent->AttachToComponent(InteractingActorMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TargetSocket);

			EquipmentComponent->MarkPickUpMeshForDestroy(HandProxy, ResolvedSlot);
		}
	}
	Super::OnInteract_Implementation(InteractingComponent);
	RemoveFromWorldObjectTracker();
}

bool AInteractablePickupObject::IsInterActionPossible_Implementation(UInteractionComponent* InteractionComponent)
{
	return Super::IsInterActionPossible_Implementation(InteractionComponent) && !bIsFalling;
}


void AInteractablePickupObject::DropFromPlayer()
{
	const USettingsObjects* ObjectSettings = GetDefault<USettingsObjects>();
	if (IsDropBlocked())
	{
		if (APawn* PlayerPawn = GetInstigator())
		{
			SetActorLocation(PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * ObjectSettings->GetFallbackDropForwardOffset());
		}
	}

	RestingCollision = StaticMeshComponent->GetCollisionEnabled();
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetNotifyRigidBodyCollision(true);


	StaticMeshComponent->BodyInstance.bGenerateWakeEvents = true;
	StaticMeshComponent->OnComponentSleep.AddDynamic(this, &AInteractablePickupObject::OnDropRest);

	if (UDefinitionEquipment* Equipment = Cast<UDefinitionEquipment>(ContainedItemDefinition))
	{
		const EEquipmentSlot Slot = Equipment->GetEquipmentSlot();
		StaticMeshComponent->SetAngularDamping(ObjectSettings->GetAngularDampingValue(Slot));
		StaticMeshComponent->SetLinearDamping(ObjectSettings->GetLinearDampingValue(Slot));
	}
	StaticMeshComponent->SetUseCCD(true);
	GetWorld()->GetTimerManager().SetTimer(DropFallbackTimer, this, &AInteractablePickupObject::FinishDrop, ObjectSettings->GetDropFallbackTimer(), false);
}

void AInteractablePickupObject::OnDropRest(UPrimitiveComponent* PrimitiveComponent, FName BoneName)
{
	FinishDrop();
}

void AInteractablePickupObject::FinishDrop()
{
	GetWorld()->GetTimerManager().ClearTimer(DropFallbackTimer);
	StaticMeshComponent->OnComponentSleep.RemoveDynamic(this, &AInteractablePickupObject::OnDropRest);
	StaticMeshComponent->SetSimulatePhysics(false);
	bIsFalling = false;
	StaticMeshComponent->SetCollisionEnabled(RestingCollision);

	if (!EventID.IsNone())
	{
		if (USubsystemObjectEvents* ObjectEventSubsystem = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
		{
			ObjectEventSubsystem->SetDroppedItemTransform(EventID, GetActorTransform());
		}
	}
}

bool AInteractablePickupObject::IsDropBlocked()
{
	const FVector SpawnLocation = GetActorLocation();
	const USettingsObjects* ObjectSettings = GetDefault<USettingsObjects>();

	if (const APawn* PlayerPawn = GetInstigator())
	{
		FCollisionShape Probe = FCollisionShape::MakeSphere(ObjectSettings->GetSafeDropRadius());
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(PlayerPawn);

		return GetWorld()->OverlapBlockingTestByChannel(SpawnLocation, FQuat::Identity, ECC_WorldStatic, Probe, Params);
	}
	return false;
}

void AInteractablePickupObject::RemoveFromWorldObjectTracker() const
{
	if (USubsystemObjectEvents* SubsystemObjectEvents = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		SubsystemObjectEvents->ClearDroppedItemTransform(EventID);
	}
}

void AInteractablePickupObject::AdjustObjectTransform()
{
	if (USubsystemObjectEvents* SubsystemObjectEvents = GetGameInstance()->GetSubsystem<USubsystemObjectEvents>())
	{
		if (SubsystemObjectEvents->HasDroppedItemLocation(EventID))
		{
			SetActorTransform(SubsystemObjectEvents->GetDroppedItemTransform(EventID));
		}
	}
}

EEquipmentSlot AInteractablePickupObject::GetSlotToSpawnIn()
{
	return EEquipmentSlot::Undefined;
}

AInteractablePickupObject* AInteractablePickupObject::SpawnHandProxy(const UDefinitionItem* ItemDefinition)
{
	if (!IsValid(ItemDefinition))
	{
		return nullptr;
	}

	UClass* PickupProxyClass = ItemDefinition->GetWorldPickUpActor().LoadSynchronous();
	if (!IsValid(PickupProxyClass))
	{
		return nullptr;
	}

	AInteractablePickupObject* HandProxy = GetWorld()->SpawnActorDeferred<AInteractablePickupObject>(PickupProxyClass, GetActorTransform());
	if (!IsValid(HandProxy))
	{
		return nullptr;
	}

	HandProxy->bIsStowProxy = true; // set before FinishSpawning so BeginPlay skips world-pickup behaviour
	HandProxy->FinishSpawning(GetActorTransform());
	return HandProxy;
}

void AInteractablePickupObject::StartStowBlend(USkeletalMeshComponent* OwnerMesh, const FName& BackupSocket, UMeshComponent* WeaponMesh, float Duration)
{
	WeaponMeshToReveal = WeaponMesh;

	if (!IsValid(OwnerMesh) || Duration <= 0.f)
	{
		FinishStowBlend();
		return;
	}

	// An den Backup-Socket umhängen, aber optisch in der Hand bleiben.                                                                                                                                                                                                                                                                                                                                                        
	StaticMeshComponent->AttachToComponent(OwnerMesh, FAttachmentTransformRules::KeepWorldTransform, BackupSocket);
	StowBlendStartRelative = StaticMeshComponent->GetRelativeTransform();
	StowBlendElapsed = 0.f;
	StowBlendDuration = Duration;
	bIsStowBlending = true;
	SetActorTickEnabled(true);
}

void AInteractablePickupObject::Destroyed()
{
	RevealWeapon();
	Super::Destroyed();
}

void AInteractablePickupObject::FinishStowBlend()
{
	bIsStowBlending = false;
	SetActorTickEnabled(false);
	RevealWeapon();
	Destroy();
}

void AInteractablePickupObject::RevealWeapon()
{
	if (bWeaponRevealed)
	{
		return;
	}
	bWeaponRevealed = true;
	if (UMeshComponent* Mesh = WeaponMeshToReveal.Get())
	{
		Mesh->SetVisibility(true);
	}
}

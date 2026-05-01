#include "CH4WeaponBase.h"

#include "Data/CH4WeaponData.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

const FName ACH4WeaponBase::HandSocketName(TEXT("Weapon_r"));

ACH4WeaponBase::ACH4WeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(DefaultSceneRoot);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponMesh->SetIsReplicated(true);
}

void ACH4WeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4WeaponBase, WeaponDataAsset);
	DOREPLIFETIME(ACH4WeaponBase, MaxClip);
	DOREPLIFETIME(ACH4WeaponBase, CurrentAmmo);
}

void ACH4WeaponBase::Equip(ACharacter* OwnerCharacter)
{
	if (!OwnerCharacter || !OwnerCharacter->GetMesh()) return;

	SetOwner(OwnerCharacter);
	SetInstigator(OwnerCharacter);

	AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		HandSocketName);

	SetActorHiddenInGame(false);
}

void ACH4WeaponBase::Unequip()
{
	SetActorHiddenInGame(true);
}

void ACH4WeaponBase::PlayWeaponShake()
{
	if (!CamShakeClass) return;

	if (WeaponPoseType == ECombatPose::BOOM)
	{
		UGameplayStatics::PlayWorldCameraShake(
			GetWorld(),
			CamShakeClass,
			GetActorLocation(),
			InnerRadius,
			OuterRadius,
			1.0f,
			false);
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->IsLocallyControlled()) return;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	float FinalScale = 1.0f;
	switch (WeaponPoseType)
	{
	case ECombatPose::Melee:      FinalScale = 0.8f; break;
	case ECombatPose::Pistol:     FinalScale = 1.0f; break;
	case ECombatPose::Shotgun:    FinalScale = 2.0f; break;
	case ECombatPose::Rifle:      FinalScale = 1.3f; break;
	case ECombatPose::MuchineGun: FinalScale = 0.7f; break;
	default: break;
	}

	PC->ClientStartCameraShake(CamShakeClass, FinalScale);
}

void ACH4WeaponBase::OnRep_WeaponData()
{
	if (!WeaponDataAsset || !WeaponMesh) return;

	WeaponMesh->SetStaticMesh(WeaponDataAsset->StaticMesh);

	WeaponMesh->SetRelativeLocation(FVector::ZeroVector);
	WeaponMesh->SetRelativeRotation(FRotator::ZeroRotator);
	WeaponMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));

	WeaponMesh->SetVisibility(true, true);
	WeaponMesh->SetHiddenInGame(false);
	WeaponMesh->MarkRenderStateDirty();
}

void ACH4WeaponBase::SetCurrentAmmo()
{
	if (WeaponDataAsset)
	{
		CurrentAmmo = WeaponDataAsset->GetMaxAmmo();
	}
}

void ACH4WeaponBase::Server_Attack_Implementation()
{
	Attack_Implementation_Internal();
}

void ACH4WeaponBase::Attack_Implementation_Internal()
{
}

void ACH4WeaponBase::Server_Reload_Implementation()
{
	Reload_Implementation_Internal();
}

void ACH4WeaponBase::Reload_Implementation_Internal()
{
}

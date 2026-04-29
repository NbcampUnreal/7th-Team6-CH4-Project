#include "CH4RangedWeapon.h"

#include "Data/CH4WeaponData.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "TimerManager.h"

ACH4RangedWeapon::ACH4RangedWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
}

void ACH4RangedWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4RangedWeapon, bIsCoolingDown);
}

void ACH4RangedWeapon::Attack_Implementation_Internal()
{
	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),
		1.0f,
		this,
		0.f,
		NAME_None);

	Attack();
}

void ACH4RangedWeapon::Reload_Implementation_Internal()
{
	if (MaxClip <= 0) return;
	ProcessReload();
}

void ACH4RangedWeapon::AddMaxClip(int32 AmmoItem)
{
	if (!HasAuthority()) return;
	MaxClip += AmmoItem;
}

void ACH4RangedWeapon::Attack_Implementation()
{
	if (!WeaponDataAsset) return;
	if (bIsCoolingDown || CurrentAmmo <= 0) return;

	bIsCoolingDown = true;
	TraceShoot();

	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;
	}

	GetWorldTimerManager().SetTimer(
		TimerHandle_FireDelay,
		this,
		&ACH4RangedWeapon::ResetCoolTime,
		WeaponDataAsset->GetFireRate(),
		false);
}

void ACH4RangedWeapon::Attack()
{
	if (bIsCoolingDown || CurrentAmmo <= 0) return;
	if (!GetOwner()) return;

	Attack_Implementation();
}

void ACH4RangedWeapon::Server_ApplyDamageToTarget_Implementation(AActor* TargetActor)
{
	if (HasAuthority())
	{
		ProcessDamage(TargetActor);
	}
}

bool ACH4RangedWeapon::Server_ApplyDamageToTarget_Validate(AActor* TargetActor)
{
	return TargetActor != nullptr;
}

void ACH4RangedWeapon::ProcessDamage(AActor* TargetActor)
{
	if (!TargetActor || !WeaponDataAsset) return;

	UGameplayStatics::ApplyDamage(
		TargetActor,
		WeaponDataAsset->Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass());

	if (GetOwner())
	{
		UAISense_Damage::ReportDamageEvent(
			GetWorld(),
			TargetActor,
			GetOwner(),
			WeaponDataAsset->Damage,
			GetOwner()->GetActorLocation(),
			TargetActor->GetActorLocation());
	}
}

void ACH4RangedWeapon::OnRep_FireReady()
{
}

void ACH4RangedWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponDataAsset)
	{
		CurrentAmmo = WeaponDataAsset->GetMaxAmmo();
	}
}

int32 ACH4RangedWeapon::GetMaxAmmo() const
{
	return WeaponDataAsset ? WeaponDataAsset->GetMaxAmmo() : 0;
}

int32 ACH4RangedWeapon::GetMaxClip() const
{
	return MaxClip;
}

void ACH4RangedWeapon::Multicast_PlayEffects_Implementation(FVector TraceStart, FVector TraceEnd, bool bHit)
{
	const FColor LineColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 2.0f, 0, 2.0f);

	if (!WeaponDataAsset) return;

	if (WeaponDataAsset->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponDataAsset->FireSound, TraceStart);
	}

	if (WeaponDataAsset->MuzzleFlash && WeaponMesh)
	{
		const FVector MuzzleLoc = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		const FRotator MuzzleRot = WeaponMesh->GetSocketRotation(TEXT("Muzzle"));

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			WeaponDataAsset->MuzzleFlash,
			MuzzleLoc,
			MuzzleRot);
	}
}

void ACH4RangedWeapon::TraceShoot()
{
	if (!HasAuthority()) return;
	if (!WeaponDataAsset) return;

	APlayerController* PC = Cast<APlayerController>(GetInstigatorController());
	if (!PC) return;

	FVector StartLocation;
	FRotator ViewRotation;
	PC->GetPlayerViewPoint(StartLocation, ViewRotation);

	const FVector EndLocation = StartLocation + (ViewRotation.Vector() * WeaponDataAsset->GetRangedLength());

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	const bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, Params);

	if (bIsHit && Hit.GetActor())
	{
		Server_ApplyDamageToTarget(Hit.GetActor());
		Multicast_PlayEffects(StartLocation, Hit.ImpactPoint, true);
	}
	else
	{
		Multicast_PlayEffects(StartLocation, EndLocation, false);
	}
}

void ACH4RangedWeapon::ProcessReload()
{
	if (!HasAuthority()) return;
	if (!WeaponDataAsset) return;
	if (CurrentAmmo >= WeaponDataAsset->GetMaxAmmo()) return;
	if (MaxClip <= 0) return;

	const int32 AmmoNeeded = WeaponDataAsset->GetMaxAmmo() - CurrentAmmo;
	const int32 AmmoToFill = FMath::Min(AmmoNeeded, MaxClip);

	CurrentAmmo += AmmoToFill;
	MaxClip -= AmmoToFill;
}

void ACH4RangedWeapon::ResetCoolTime()
{
	bIsCoolingDown = false;
}

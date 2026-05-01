#include "CH4MeleeWeapon.h"

#include "Data/CH4MeleeWeaponData.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACH4MeleeWeapon::ACH4MeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	MeleeDetector = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeDetector"));
	MeleeDetector->SetupAttachment(RootComponent);
	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeleeDetector->OnComponentBeginOverlap.AddDynamic(this, &ACH4MeleeWeapon::OnMeleeOverlap);
}

void ACH4MeleeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4MeleeWeapon, MeleeAttackMontage);
}

void ACH4MeleeWeapon::Attack_Implementation_Internal()
{
	Super::Attack_Implementation_Internal();
	Attack();
}

void ACH4MeleeWeapon::Attack()
{
	Multicast_PlayAttackMontage();
}

void ACH4MeleeWeapon::EnableMeleeCollision()
{
	if (!HasAuthority())
	{
		Server_EnableMeleeCollision();
		return;
	}

	HitActors.Empty();
	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ACH4MeleeWeapon::DisableMeleeCollision()
{
	if (!HasAuthority())
	{
		Server_DisableMeleeCollision();
		return;
	}

	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACH4MeleeWeapon::Multicast_PlayAttackMontage_Implementation()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter && MeleeAttackMontage)
	{
		OwnerCharacter->PlayAnimMontage(MeleeAttackMontage, 1.5f);
	}
}

void ACH4MeleeWeapon::Server_EnableMeleeCollision_Implementation()
{
	HitActors.Empty();
	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ACH4MeleeWeapon::Server_DisableMeleeCollision_Implementation()
{
	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACH4MeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (MeleeData && MeleeDetector)
	{
		MeleeDetector->SetSphereRadius(MeleeData->AttackRadius);
		MeleeDetector->SetHiddenInGame(true);
	}
}

void ACH4MeleeWeapon::OnMeleeOverlap(
	UPrimitiveComponent* /*OverlappedComponent*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/,
	bool /*bFromSweep*/,
	const FHitResult& /*SweepResult*/)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == GetOwner()) return;
	if (HitActors.Contains(OtherActor)) return;
	if (!OtherActor->ActorHasTag(TEXT("Enemy"))) return;
	if (!MeleeData) return;

	HitActors.Add(OtherActor);
	UGameplayStatics::ApplyDamage(OtherActor, MeleeData->Damage, GetInstigatorController(), this, nullptr);
}

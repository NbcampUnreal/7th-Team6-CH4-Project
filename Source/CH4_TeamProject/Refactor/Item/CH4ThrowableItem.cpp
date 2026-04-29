#include "CH4ThrowableItem.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

ACH4ThrowableItem::ACH4ThrowableItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->InitSphereRadius(15.0f);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Block);
	SphereComp->SetNotifyRigidBodyCollision(true);
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereComp->OnComponentHit.AddDynamic(this, &ACH4ThrowableItem::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
}

void ACH4ThrowableItem::Explode()
{
	if (!HasAuthority()) return;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	if (GetOwner())
	{
		IgnoreActors.Add(GetOwner());
	}

	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		ExplosionDamage,
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		IgnoreActors,
		this,
		GetInstigatorController(),
		true);

	Multicast_PlayExplosionEffects();
	Destroy();
}

void ACH4ThrowableItem::Multicast_PlayExplosionEffects_Implementation()
{
	if (ExplosionEffect)
	{
		const FVector SpawnLocation = GetActorLocation();
		const FRotator SpawnRotation = GetActorRotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			SpawnLocation,
			SpawnRotation,
			FVector(1.0f),
			true,
			true,
			ENCPoolMethod::None,
			true);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}

void ACH4ThrowableItem::OnHit(
	UPrimitiveComponent* /*HitComponent*/,
	AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/,
	FVector /*NormalImpulse*/,
	const FHitResult& /*Hit*/)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == this) return;
}

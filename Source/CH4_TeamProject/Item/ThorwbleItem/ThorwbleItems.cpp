#include "ThorwbleItems.h"

#include "CH4_TeamProject/Zombie/ZombieBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AThorwbleItems::AThorwbleItems()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	AActor::SetReplicateMovement(true);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->InitSphereRadius(15.0f);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Block);
	SphereComp->SetNotifyRigidBodyCollision(true);
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereComp->OnComponentHit.AddDynamic(this, &AThorwbleItems::OnHit);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
}

void AThorwbleItems::BeginPlay()
{
	Super::BeginPlay();
}

void AThorwbleItems::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AThorwbleItems::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                           UPrimitiveComponent* OtherComp,
                           FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == this) return;
}

void AThorwbleItems::Explode()
{
	if (!HasAuthority()) return;
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	if (GetOwner()) IgnoreActors.Add(GetOwner());

	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		ExplosionDamage,
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		IgnoreActors,
		this,
		GetInstigatorController(),
		true
	);

	Multi_PlayExplosionEffects();
	Destroy();
}

void AThorwbleItems::Multi_PlayExplosionEffects_Implementation()
{
	if (ExplosionEffect) 
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			SpawnLocation,
			SpawnRotation,
			FVector(1.0f), 
			true,          
			true,          
			ENCPoolMethod::None,
			true
		);
	}
	
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}

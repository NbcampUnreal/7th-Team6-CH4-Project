#include "ThorwbleItems.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

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
	UE_LOG(LogTemp, Warning, TEXT("OnHit 발생! OtherActor: %s"),
	       OtherActor ? *OtherActor->GetName() : TEXT("None"));

	if (!HasAuthority()) return;
	if (!OtherActor || OtherActor == this) return;

	if (IsA(APawn::StaticClass()))
	{
		Explode();
	}
}

void AThorwbleItems::Explode()
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Error, TEXT("Explode() 호출됨! 위치: %s"), *GetActorLocation().ToString());

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
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), ExplosionEffect, GetActorLocation());
	}
	UE_LOG(LogTemp, Error, TEXT("수류탄 터짐"));
}

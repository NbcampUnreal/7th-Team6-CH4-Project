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
	if (ExplosionEffect) // 데이터 에셋이나 헤더에 선언된 나이아가라 시스템
	{
		// 현재 수류탄이 있는 바로 그 위치!
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			SpawnLocation,
			SpawnRotation,
			FVector(1.0f), // 크기 조절 (더 크게 하고 싶으면 2.0f ㅋㅋㅋ)
			true,          // Auto Destroy (다 재생되면 메모리에서 삭제 ㅡㅡb)
			true,          // Auto Activate
			ENCPoolMethod::None,
			true
		);
	}
	
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	UE_LOG(LogTemp, Error, TEXT("수류탄 터짐"));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeapons.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARangedWeapons::ARangedWeapons()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	
	PrimaryActorTick.bCanEverTick = false;
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	
	SetRootComponent(WeaponMesh);
	
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	
	DataAsset = CreateDefaultSubobject<URangedWeaponDataAsset>(TEXT("DataAsset"));
	
	CurrentAmmo = DataAsset->MaxAmmo;
}


void ARangedWeapons::Server_ApplyDamageToTarget_Implementation(AActor* TargetActor)
{
	if (HasAuthority())
	{
		ProcessDamage(TargetActor);
	}
}

bool ARangedWeapons::Server_ApplyDamageToTarget_Validate(AActor* TargetActor)
{
	return TargetActor != nullptr;
}

void ARangedWeapons::ProcessDamage(AActor* TargetActor)
{
	if (TargetActor)
	{
		// 중요: ApplyDamage를 호출하면 피격자의 TakeDamage가 서버에서 자동 호출됨
		UGameplayStatics::ApplyDamage(
			TargetActor,           
			DataAsset->Damage,          
			GetInstigatorController(), 
			this,                  
			UDamageType::StaticClass() 
		);
	}
}

void ARangedWeapons::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// float ARangedWeapons::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
// 	class AController* EventInstigator, AActor* DamageCauser)
// {
// 	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
// 	
// 	if (HasAuthority())
// 	{
// 		
// 	}
// 	
// }

// Called when the game starts or when spawned
void ARangedWeapons::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARangedWeapons::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARangedWeapons::Server_Fire_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("3. Server_Fire_Implementation"));
	TraceShoot();
	Multicast_PlayEffects();
	// 총알 감소및
	// 쿨타임 초기화 등등
}

bool ARangedWeapons::Server_Fire_Validate()
{
	UE_LOG(LogTemp, Warning, TEXT("2. Server_Fire_Validate"));
	// 조건확인 총알갯수 
	// 쿨타임 등등
	// 이결과가 참이여야 위에 함수가 실행
	return true;
}

void ARangedWeapons::Multicast_PlayEffects_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Fire Effects Played!"));
}

// 시작점
void ARangedWeapons::Fire()
{
	if (GetOwner() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("사격 실패: 이 무기의 Owner가 설정되지 않았습니다!"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("1. Fire Called on Client"));
	// 서버에 사격요청및 로컬에서만 이펙트 사운드 재생
	
	// if (IsLocallyControlled())
	// {
	// 	Multicast_PlayEffects(); 
	// }
	
	Server_Fire();	
}

void ARangedWeapons::TraceShoot()
{
	// 서버 권한이 없으면 실행하지 않음 (이중 보안)
	if (!HasAuthority()) return;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + (GetActorForwardVector() * 5000.f);

	bool bIsHit =  GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, Params);
	FColor LineColor = bIsHit ? FColor::Red : FColor::Green; // 맞으면 빨강, 안 맞으면 초록
    
	DrawDebugLine(
		GetWorld(),
		StartLocation,
		bIsHit ? Hit.ImpactPoint : EndLocation, // 맞은 지점까지만 그리거나 끝까지 그림
		LineColor,
		false,      // Persistent (영구 유지 여부)
		2.0f,       // LifeTime (2초 동안 화면에 남음)
		0,          // DepthPriority
		2.0f        // Thickness (선의 두께)
	);
	if (bIsHit)
	{
		// 서버니까 당당하게 Destroy()나 데미지 처리를 합니다.
		if (Hit.GetActor())
		{
			Server_ApplyDamageToTarget(Hit.GetActor());
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "RangedWeapons.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RangedWeaponDataAsset.h"
#include "CH4_TeamProject/Zombie/ZombieBase.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
ARangedWeapons::ARangedWeapons()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
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
			WeaponData->Damage,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);


		UAISense_Damage::ReportDamageEvent(
			GetWorld(),
			TargetActor,
			GetOwner(), // 공격자
			WeaponData->Damage,
			GetOwner()->GetActorLocation(),
			TargetActor->GetActorLocation()
		);
	}
}

void ARangedWeapons::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARangedWeapons, CurrentAmmo);
	DOREPLIFETIME(ARangedWeapons, MaxClip);
	DOREPLIFETIME(ARangedWeapons, bIsCoolingDown);
}


void ARangedWeapons::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponData)
	{
		//CurrentAmmo = Data->GetMaxAmmo();
		CurrentAmmo = WeaponData->GetMaxAmmo();

		UE_LOG(LogTemp, Warning, TEXT("MaxAmmo: %d"), WeaponData-> GetMaxAmmo());
		UE_LOG(LogTemp, Warning, TEXT("CurrentAmmo: %d"), CurrentAmmo);
		UE_LOG(LogTemp, Warning, TEXT("GetMaxAmmo(): %d"), GetMaxAmmo());
	}
	else { UE_LOG(LogTemp, Warning, TEXT("데이터 없다")) }
}

// Called every frame
void ARangedWeapons::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	{
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, TEXT("Yahoo: RangedWeapon is Ticking!"));
	}
}

void ARangedWeapons::Attack_Implementation_Internal()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString(TEXT("자식 호출됨")), true);

	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),
		1.0f,
		this,
		0.f,
		NAME_None
	);

	Attack();
}

void ARangedWeapons::Multicast_PlayEffects_Implementation(FVector TraceStart, FVector TraceEnd, bool bHit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Multicast 실행됨!"));

	FColor LineColor = bHit ? FColor::Red : FColor::Green;
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 2.0f, 0, 2.0f);

	if (!WeaponData) return; // ← 하나로 통일

	// 사운드
	if (WeaponData->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponData->FireSound, TraceStart);
	}

	// 머즐플래시
	if (WeaponData->MuzzleFlash)
	{
		if (!WeaponMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("컴포넌트 없음"));
			return;
		}

		FVector MuzzleLoc = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
		FRotator MuzzleRot = WeaponMesh->GetSocketRotation(TEXT("Muzzle"));

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			WeaponData->MuzzleFlash,
			MuzzleLoc,
			MuzzleRot
		);
	}
}

// 시작점
void ARangedWeapons::Attack()
{
	UE_LOG(LogTemp, Error, TEXT("무조건 찍혀야 하는 로그!"));
	if (bIsCoolingDown || CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("총알이 부족해 혹은 딜레이중이야 현재 총알 :%d"), CurrentAmmo);
		return;
	}
	if (GetOwner() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("사격 실패: 이 무기의 Owner가 설정되지 않았습니다!"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("1. Fire Called on Client"));
	Attack_Implementation();
}


void ARangedWeapons::ProcessReload()
{
	if (!HasAuthority())
	{
		return;
	}
	if (!WeaponData || CurrentAmmo >= WeaponData->GetMaxAmmo()) return;
	if (MaxClip <= 0) { return; }
	int32 AmmoNeeded = WeaponData->GetMaxAmmo() - CurrentAmmo;
	int32 AmmoToFill = FMath::Min(AmmoNeeded, MaxClip);

	if (HasAuthority() && WeaponData)
	{
		CurrentAmmo += AmmoToFill;
		MaxClip -= AmmoToFill;
	}
}

void ARangedWeapons::TraceShoot()
{
	// 서버 권한이 없으면 실행하지 않음 (이중 보안)
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("서버가 아님"))
		return;
	}
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT(" 데이터가 유효하지않음 "))
		return;
	}
	APlayerController* PC = Cast<APlayerController>(GetInstigatorController());
	if (!PC) { return; }

	FVector StartLocation;
	FRotator ViewRotation;

	PC->GetPlayerViewPoint(StartLocation, ViewRotation);
	FVector EndLocation = StartLocation + (ViewRotation.Vector() * WeaponData->GetRangedLength());

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, Params);
	FColor LineColor = bIsHit ? FColor::Red : FColor::Green; // 맞으면 빨강, 안 맞으면 초록
	
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

// 데이터 에셋에서 최대 장전 발수
int32 ARangedWeapons::GetMaxAmmo() const
{
	if (WeaponData)
	{
		return WeaponData->GetMaxAmmo();
	}
	return 0;
}

// 현재 최대들고있는 총알갯수
int32 ARangedWeapons::GetMaxClip() const
{
	return MaxClip;
}

void ARangedWeapons::OnRep_FireReady()
{
}

void ARangedWeapons::ResetCoolTime()
{
	bIsCoolingDown = false;

	UE_LOG(LogTemp, Error, TEXT("Yahoo: 쿨다운 해제됨!"));
}

void ARangedWeapons::AddMaxClip(int32 AmmoItem)
{
	if (!HasAuthority())
	{
		return;
	}
	MaxClip += AmmoItem;
}


void ARangedWeapons::Reload_Implementation_Internal()
{
	if (MaxClip <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("MaxClip 없어서 리턴됨")) // ← 추가
		return;
	}
	ProcessReload();
	UE_LOG(LogTemp, Error, TEXT(" 자식 장전 호출됨"))
}

void ARangedWeapons::Attack_Implementation()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT(" 데이터 유호하지않음"))
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Server_Fire 실행됨!"));


	if (bIsCoolingDown || CurrentAmmo <= 0)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString::Printf(TEXT("총알이없습니다")));
		return;
	}
	bIsCoolingDown = true;
	TraceShoot();
	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;
	}
	GEngine->AddOnScreenDebugMessage(-1,
	                                 10, FColor::Red,
	                                 FString::Printf(
		                                 TEXT("남은 총알 %d"), CurrentAmmo)
	                                 , false);
	// 총알 감소및
	// 쿨타임 초기화 등등
	GetWorldTimerManager().SetTimer
	(TimerHandle_FireDelay,
	 this,
	 &ARangedWeapons::ResetCoolTime,
	 WeaponData->GetFireRate(),
	 false);
}

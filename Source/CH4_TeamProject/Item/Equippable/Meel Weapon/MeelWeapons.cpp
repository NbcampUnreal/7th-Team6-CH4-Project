// Fill out your copyright notice in the Description page of Project Settings.


#include "MeelWeapons.h"
#include "MeelWeaponData.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMeleeWeapons::AMeleeWeapons()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	MeleeDetector = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeDetector"));
	MeleeDetector->SetupAttachment(RootComponent);
	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeleeDetector->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapons::OnMeleeOverlap);
}

// Called when the game starts or when spawned
void AMeleeWeapons::BeginPlay()
{
	Super::BeginPlay();
	if (MeelData && MeleeDetector)
	{
		MeleeDetector->SetSphereRadius(MeelData->AttackRadius);
		MeleeDetector->SetHiddenInGame(false); 
	}
}

void AMeleeWeapons::OnMeleeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	if (OtherActor && OtherActor != GetOwner() && !HitActors.Contains(OtherActor))
	{
		// 태그 애너미 만 때리기인데 
		if (OtherActor->ActorHasTag(TEXT("Enemy")))
		{
			HitActors.Add(OtherActor); // 목록에 추가
			
			UGameplayStatics::ApplyDamage(OtherActor, MeelData->Damage, GetInstigatorController(), this, nullptr);
            
			UE_LOG(LogTemp, Warning, TEXT("%s에게 %d 데미지!"), *OtherActor->GetName(),MeelData-> Damage);
		}
	}
}

void AMeleeWeapons::EnableMeleeCollision()
{
	if (HasAuthority())
	{
		HitActors.Empty(); 
		MeleeDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AMeleeWeapons::DisableMeleeCollision()
{
	if (HasAuthority())
	{
		MeleeDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMeleeWeapons::Attack()
{
	//
}




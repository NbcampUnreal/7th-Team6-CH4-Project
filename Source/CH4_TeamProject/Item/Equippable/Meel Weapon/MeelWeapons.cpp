// Fill out your copyright notice in the Description page of Project Settings.


#include "MeelWeapons.h"
#include "MeelWeaponData.h"
#include "CH4_TeamProject/Player/PlayerAnimInstance.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
			HitActors.Add(OtherActor); 
			
			UGameplayStatics::ApplyDamage(OtherActor, MeelData->Damage, GetInstigatorController(), this, nullptr);
            
			UE_LOG(LogTemp, Warning, TEXT("%s에게 %d 데미지!"), *OtherActor->GetName(),MeelData-> Damage);
		}
	}
}


void AMeleeWeapons::Attack_Implementation_Internal()
{
	Super::Attack_Implementation_Internal();
	Attack();
}

void AMeleeWeapons::Attack()
{
	Multicast_PlayAttackMontage();
}

void AMeleeWeapons::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


void AMeleeWeapons::EnableMeleeCollision()
{
	if (!HasAuthority())
	{
		Server_EnableMeleeCollision();
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("EnableMeleeCollision 호출 - %s"), 
	   HasAuthority() ? TEXT("Server") : TEXT("Client"));
		HitActors.Empty(); 
		MeleeDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMeleeWeapons::Server_EnableMeleeCollision_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server_EnableMeleeCollision 실행됨"));
	HitActors.Empty();
	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
void AMeleeWeapons::DisableMeleeCollision()
{
	if (!HasAuthority())
	{
		Server_DisableMeleeCollision();
		return;
	}
		MeleeDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMeleeWeapons::Server_DisableMeleeCollision_Implementation()
{
	MeleeDetector->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



void AMeleeWeapons::Multicast_PlayAttackMontage_Implementation()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    
	if (OwnerCharacter && MeleeAttackMontage)
	{
		OwnerCharacter->PlayAnimMontage(MeleeAttackMontage,1.5);
		UE_LOG(LogTemp, Warning, TEXT("PlayAnimMontage Called on: %s"), 
			   OwnerCharacter->HasAuthority() ? TEXT("Server") : TEXT("Client"));
		UE_LOG(LogTemp,Error,TEXT("근접공격 실행됨"))
	}
}




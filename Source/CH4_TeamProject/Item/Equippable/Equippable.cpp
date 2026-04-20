// Fill out your copyright notice in the Description page of Project Settings.


#include "Equippable.h"

#include "WeaponData.h"
#include "GameFramework/Character.h"


// Sets default values
AEquippable::AEquippable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEquippable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEquippable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEquippable::Attack_Implementation_Internal()
{
	
}

void AEquippable::Reload_Implementation_Internal()
{
	
}

void AEquippable::Server_Reload_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("실제 클래스: %s"), *GetClass()->GetName());
	Reload_Implementation_Internal();
}

void AEquippable::Server_Attack_Implementation()
{
	Attack_Implementation_Internal();
	UE_LOG(LogTemp,Error,TEXT("자식꺼 공격 오버라이드 가능?"))
}




void AEquippable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEquippable::SetCurrentAmmo()
{
	if (WeaponData)
	{
		CurrentAmmo = WeaponData->GetMaxAmmo();
	}
}





// Fill out your copyright notice in the Description page of Project Settings.


#include "RPCTestActor.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ARPCTestActor::ARPCTestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ARPCTestActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARPCTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ARPCTestActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
	if (HasAuthority())
	{
		Hp -= ActualDamage;
		if (Hp <=0)
		{
			Destroy();
		}
	}
	UE_LOG(LogTemp,Error, TEXT("%d"),Hp);
	return ActualDamage;
}

void ARPCTestActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 총알배송할 변수등록
	DOREPLIFETIME(ARPCTestActor, Hp);
}

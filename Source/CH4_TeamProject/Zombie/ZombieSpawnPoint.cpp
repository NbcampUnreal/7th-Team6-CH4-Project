// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSpawnPoint.h"

AZombieSpawnPoint::AZombieSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AZombieSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

void AZombieSpawnPoint::SpawnZombie()
{
	if (!GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	auto SpawnHelper = [&](TSubclassOf<AActor> Class, int32 Min, int32 Max)
	{
		if (!Class) return;
		int32 Count = FMath::RandRange(Min, Max);
		for (int32 i = 0; i < Count; ++i)
		{
			FVector Location = GetActorLocation();
			Location.X += FMath::RandRange(-1200.f, 1200.f);  
			Location.Y += FMath::RandRange(-1200.f, 1200.f);  
			Location.Z += 60.f;

			FRotator Rotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);
			AActor* Spawned = GetWorld()->SpawnActor<AActor>(Class, Location, Rotation, SpawnParams);

			if (Spawned)
			{
				UE_LOG(LogTemp, Log, TEXT("%s 스폰 성공!"), *Class->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("%s 스폰 실패"), *Class->GetName());
			}
		}
	};

	SpawnHelper(NormalZombieClass, 8, 10);
	SpawnHelper(RunnerZombieClass, 4, 6);
	SpawnHelper(TankZombieClass, 1, 1);
}
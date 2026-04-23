// Fill out your copyright notice in the Description page of Project Settings.
#include "ItemSpawnPoint.h"
#include "USpawnDataAsset.h"
#include "Engine/World.h"


AItemSpawnPoint::AItemSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AItemSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

void AItemSpawnPoint::SpawnItems()
{
	if (!SpawnItemData)
	{
		return;
	}
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
			Location.X += FMath::RandRange(-500.f, 500.f);  
			Location.Y += FMath::RandRange(-500.f, 500.f);  
			Location.Z += 5.f;

			FRotator Rotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);
			AActor* Spawned = GetWorld()->SpawnActor<AActor>(Class, Location, Rotation, SpawnParams);
		}
	};	

	SpawnHelper(AmmoClass, SpawnItemData->MinAmmoItem, SpawnItemData->MaxAmmoItem);
	SpawnHelper(HealClass, SpawnItemData->MinHeelItem, SpawnItemData->MaxHeelItem);
	SpawnHelper(GrenadeClass, SpawnItemData->MinGrenadeItem, SpawnItemData->MaxGrenadeItem);
}

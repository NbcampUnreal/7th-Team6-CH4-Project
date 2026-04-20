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
		UE_LOG(LogTemp, Error, TEXT("데이터가 없다"));
		return;
	}
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("월드가 없다"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	auto SpawnHelper = [&](TSubclassOf<AActor> Class, int32 Min, int32 Max)
	{
		int32 SpawnFalseCount = 0;
		if (!Class) return;
		int32 Count = FMath::RandRange(Min, Max);
		for (int32 i = 0; i < Count; ++i)
		{
			FVector Location = GetActorLocation();
			Location.X += FMath::RandRange(-200.f, 200.f);  
			Location.Y += FMath::RandRange(-200.f, 200.f);  
			Location.Z += 5.f;

			FRotator Rotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);
			AActor* Spawned = GetWorld()->SpawnActor<AActor>(Class, Location, Rotation, SpawnParams);

			if (Spawned)
			{
				UE_LOG(LogTemp, Log, TEXT("%s 스폰 성공!"), *Class->GetName());
			}
			else
			{
				SpawnFalseCount++;
				UE_LOG(LogTemp, Error, TEXT("%s 스폰 실패"), *Class->GetName());
			}
		}
		
		for (int32 i = 0; i < SpawnFalseCount; ++i)
		{
			// 스폰 실패한 아이템종류 구해서 다시스폰??
		}
	};	

	SpawnHelper(AmmoClass, SpawnItemData->MinAmmoItem, SpawnItemData->MaxAmmoItem);
	SpawnHelper(HealClass, SpawnItemData->MinHeelItem, SpawnItemData->MaxHeelItem);
	SpawnHelper(GrenadeClass, SpawnItemData->MinGrenadeItem, SpawnItemData->MaxGrenadeItem);
}

#include "CH4ItemSpawnPoint.h"

#include "../Item/CH4ItemBase.h"
#include "Data/CH4SpawnPointData.h"
#include "Engine/World.h"

ACH4ItemSpawnPoint::ACH4ItemSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACH4ItemSpawnPoint::SpawnItems()
{
	if (!SpawnItemData) return;
	if (!GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	auto SpawnHelper = [&](TSubclassOf<ACH4ItemBase> Class, int32 Min, int32 Max)
	{
		if (!Class) return;
		const int32 Count = FMath::RandRange(Min, Max);
		for (int32 i = 0; i < Count; ++i)
		{
			FVector Location = GetActorLocation();
			Location.X += FMath::RandRange(-500.f, 500.f);
			Location.Y += FMath::RandRange(-500.f, 500.f);
			Location.Z += 5.f;

			const FRotator Rotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);
			GetWorld()->SpawnActor<ACH4ItemBase>(Class, Location, Rotation, SpawnParams);
		}
	};

	SpawnHelper(AmmoClass, SpawnItemData->MinAmmoItem, SpawnItemData->MaxAmmoItem);
	SpawnHelper(HealClass, SpawnItemData->MinHealItem, SpawnItemData->MaxHealItem);
	SpawnHelper(GrenadeClass, SpawnItemData->MinGrenadeItem, SpawnItemData->MaxGrenadeItem);
}

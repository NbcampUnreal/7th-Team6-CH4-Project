#include "CH4ZombieSpawnPoint.h"

#include "Data/CH4ZombieSpawnData.h"
#include "CH4_TeamProject/Refactor/Monster/Character/CH4MonsterBase.h"
#include "Engine/World.h"

ACH4ZombieSpawnPoint::ACH4ZombieSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACH4ZombieSpawnPoint::SpawnZombies(UCH4ZombieSpawnData* OverrideData)
{
	UWorld* World = GetWorld();
	if (!World) return;

	const UCH4ZombieSpawnData* Data = OverrideData ? OverrideData : SpawnData.Get();
	if (!Data) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FVector Origin = GetActorLocation();
	const float Radius = Data->SpawnRadius;
	const float HeightOffset = Data->SpawnHeightOffset;

	for (const FCH4ZombieSpawnEntry& Entry : Data->Entries)
	{
		if (!Entry.ZombieClass) continue;

		const int32 Count = FMath::RandRange(Entry.MinCount, Entry.MaxCount);
		for (int32 i = 0; i < Count; ++i)
		{
			FVector Location = Origin;
			Location.X += FMath::RandRange(-Radius, Radius);
			Location.Y += FMath::RandRange(-Radius, Radius);
			Location.Z += HeightOffset;

			const FRotator Rotation(0.f, FMath::RandRange(0.f, 360.f), 0.f);
			World->SpawnActor<ACH4MonsterBase>(Entry.ZombieClass, Location, Rotation, SpawnParams);
		}
	}
}

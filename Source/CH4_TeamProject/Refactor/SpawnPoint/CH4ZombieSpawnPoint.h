#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CH4ZombieSpawnPoint.generated.h"

class ACH4MonsterBase;
class UCH4ZombieSpawnData;

UCLASS()
class CH4_TEAMPROJECT_API ACH4ZombieSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ACH4ZombieSpawnPoint();

	// OverrideData == nullptr 이면 멤버 SpawnData 사용.
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnZombies(UCH4ZombieSpawnData* OverrideData = nullptr);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TObjectPtr<UCH4ZombieSpawnData> SpawnData;
};

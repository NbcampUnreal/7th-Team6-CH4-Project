#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CH4ItemSpawnPoint.generated.h"

class ACH4ItemBase;
class UCH4SpawnPointData;

UCLASS()
class CH4_TEAMPROJECT_API ACH4ItemSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ACH4ItemSpawnPoint();

	void SpawnItems();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TObjectPtr<UCH4SpawnPointData> SpawnItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ACH4ItemBase> GrenadeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ACH4ItemBase> HealClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ACH4ItemBase> AmmoClass;
};

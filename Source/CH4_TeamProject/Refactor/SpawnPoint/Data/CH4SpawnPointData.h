#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CH4SpawnPointData.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4SpawnPointData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn", meta = (ClampMin = "0"))
	int32 MinHealItem = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn", meta = (ClampMin = "0"))
	int32 MaxHealItem = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn", meta = (ClampMin = "0"))
	int32 MinAmmoItem = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn", meta = (ClampMin = "0"))
	int32 MaxAmmoItem = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn", meta = (ClampMin = "0"))
	int32 MinGrenadeItem = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawn", meta = (ClampMin = "0"))
	int32 MaxGrenadeItem = 0;
};

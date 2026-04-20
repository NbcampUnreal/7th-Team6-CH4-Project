// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "USpawnDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API USpawnDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 MinHeelItem = 0;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 MaxHeelItem = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 MinAmmoItem =0;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 MaxAmmoItem =0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 MinGrenadeItem = 0 ;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 MaxGrenadeItem = 0 ;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 Weapon =0 ;
};

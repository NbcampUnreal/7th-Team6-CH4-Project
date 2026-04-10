// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../../Type.h"
#include "ConsumableDataAsset.generated.h"
/**
 * 
 */


UCLASS()
class CH4_TEAMPROJECT_API UConsumableDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectType Type; 
};

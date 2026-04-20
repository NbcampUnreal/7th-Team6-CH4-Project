// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnPoint.generated.h"

class USpawnDataAsset;

UCLASS()
class CH4_TEAMPROJECT_API AItemSpawnPoint : public AActor
{
	GENERATED_BODY()
public:
	AItemSpawnPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	USpawnDataAsset* SpawnItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> GrenadeClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> HealClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> AmmoClass;
	
	void SpawnItems();
protected:
	virtual void BeginPlay() override;

private:
};
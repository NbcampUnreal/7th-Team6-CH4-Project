// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieSpawnPoint.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AZombieSpawnPoint : public AActor
{
	GENERATED_BODY()
public:
	AZombieSpawnPoint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> NormalZombieClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> RunnerZombieClass;
	
	void SpawnZombie();
	
protected:
	virtual void BeginPlay() override;
};
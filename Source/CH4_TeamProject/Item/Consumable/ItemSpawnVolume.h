// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnVolume.generated.h"

// 클래스 전방 선언
class UBoxComponent;

UCLASS()
class CH4_TEAMPROJECT_API AItemSpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemSpawnVolume();

protected:
	virtual void BeginPlay() override;
	
	FVector GetRandomPointInVolume() const;
	
	FVector GetSafeSpawnLocation();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawnBox;
	
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TArray<TSubclassOf<AActor>> NormalItems;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> SpecialItemClass;


	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SafeRadius = 100.0f;
	
	void SpawnAllItems();
};
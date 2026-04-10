// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsumableItem.h"
#include "HealItem.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AHealItem : public AConsumableItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHealItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	int32 HealAmount = 20;
};

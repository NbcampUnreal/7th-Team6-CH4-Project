// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsumableItem.h"
#include "GearItem.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AGearItem : public AConsumableItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGearItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class UConsumableDataAsset> DataAsset;
};

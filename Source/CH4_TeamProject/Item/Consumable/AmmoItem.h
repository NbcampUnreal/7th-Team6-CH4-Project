// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsumableItem.h"
#include "AmmoItem.generated.h"

class ARangedWeapons; 
UCLASS()	
class CH4_TEAMPROJECT_API AAmmoItem : public AConsumableItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAmmoItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Ammo")
	TObjectPtr<class UConsumableDataAsset> DataAsset;
	
	void NotifyActorBeginOverlap(AActor* OtherActor);
	
	UPROPERTY()
	ARangedWeapons* Weapons;
};

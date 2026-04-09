// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseItem.h"
#include "Equippable.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AEquippable : public ABaseItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEquippable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};

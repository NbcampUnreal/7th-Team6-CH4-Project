// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquippableComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH4_TEAMPROJECT_API UEquippableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquippableComponent();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<int32,class URangedGunDataAsset*> DataAsset;

};

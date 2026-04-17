// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
UCLASS()
class CH4_TEAMPROJECT_API ABaseItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseItem();

protected:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,Category= "Component")
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere,Category= "Component")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	class UConsumableDataAsset* ItemData;
};

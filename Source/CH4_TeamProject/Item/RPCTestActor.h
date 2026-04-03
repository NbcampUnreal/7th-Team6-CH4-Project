// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPCTestActor.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ARPCTestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARPCTestActor();
	
	UPROPERTY(Replicated)
	int32 Hp = 100;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

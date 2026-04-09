// Fill out your copyright notice in the Description page of Project Settings.


#include "Equippable.h"

#include "GameFramework/Character.h"


// Sets default values
AEquippable::AEquippable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEquippable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEquippable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




// Fill out your copyright notice in the Description page of Project Settings.


#include "GearItem.h"


// Sets default values
AGearItem::AGearItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGearItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGearItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


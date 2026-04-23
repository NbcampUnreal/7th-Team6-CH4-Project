// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsumableItemComponent.h"


// Sets default values for this component's properties
UConsumableItemComponent::UConsumableItemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UConsumableItemComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UConsumableItemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


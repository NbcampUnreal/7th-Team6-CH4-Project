// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4Character.h"

// Sets default values
ACH4Character::ACH4Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACH4Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACH4Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACH4Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


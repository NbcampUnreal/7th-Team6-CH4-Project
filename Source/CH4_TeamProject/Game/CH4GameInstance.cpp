#include "CH4GameInstance.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4GameInstance.h"

UCH4GameInstance::UCH4GameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}


void UCH4GameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Total Score Updated: %d"), TotalScore);
}
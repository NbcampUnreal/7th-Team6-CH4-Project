#include "CH4DefenseGameInstance.h"

UCH4DefenseGameInstance::UCH4DefenseGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UCH4DefenseGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Total Score Updated: %d"), TotalScore);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Game/CH4GameMode.h"
#include "FinalDefenceManager.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API AFinalDefenceManager : public ACH4GameMode
{
	GENERATED_BODY()
	
public:
	AFinalDefenceManager();
	~AFinalDefenceManager();
	
public:
	void StartFinalDefenceTimerCountDown();
};

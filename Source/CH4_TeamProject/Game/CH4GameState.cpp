// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4GameState.h"
#include "CH4GameInstance.h"


void ACH4GameState::AddScore(int32 Amount)
{

	Score += Amount;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UCH4GameInstance* CH4GameInstance = Cast<UCH4GameInstance>(GameInstance);
		if (CH4GameInstance)
		{
			CH4GameInstance->AddToScore(Amount);
		}
	}
}


ACH4GameState::ACH4GameState()
{
	// 기본값 초기화 (예: Score 초기화)
	Score = 0;
	LevelDuration = 60.0f;
	MaxLevels = 2;
}

int32 ACH4GameState::GetScore() const
{
	return Score; // 혹은 저장된 변수 반환
}



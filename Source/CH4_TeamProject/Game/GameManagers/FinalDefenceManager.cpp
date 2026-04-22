// Fill out your copyright notice in the Description page of Project Settings.


#include "FinalDefenceManager.h"

#include "CH4_TeamProject/Game/CH4GameState.h"

AFinalDefenceManager::AFinalDefenceManager()
{
}

AFinalDefenceManager::~AFinalDefenceManager()
{
}

void AFinalDefenceManager::StartFinalDefenceTimer()
{
	// todo : 
	// FinalDefenceWaveTimer 시작
	
	// UI 갱신 명령
	// 주변의 좀비들이 몰려오도록 좀비의 탐지 범위 증가
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (!GS) return;
	
	if (FinalDefenceWaveTimer >=  6.f * 60.f || GS->AlivePlayerCount <= 0)
	{
		SetGameResult();
	}
}

void AFinalDefenceManager::StartFinalDefenceWave()
{
}

void AFinalDefenceManager::SetZombieMoveToSpot()
{
}
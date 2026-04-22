// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Game/CH4GameMode.h"
#include "FinalDefenceManager.generated.h"

/*
 * <Todo>
 * - 수정 : 기어 3개 모을 시 UI 띄우기(구조 요청을 통해 탈출하세요!)
 * - 낮밤 타이머를 끄기? or 그걸 활용해서 시간 카운트
 * - 좀비 스폰량 초기화 및 구조 지점으로 몰려오도록
 * - AI Constroller 탐지 범위 늘리기
 * - 타이머 끝날 시 EndGame() 호출
 */

UCLASS()
class CH4_TEAMPROJECT_API AFinalDefenceManager : public ACH4GameMode
{
	GENERATED_BODY()
	
public:
	AFinalDefenceManager();
	~AFinalDefenceManager();

private:
	float FinalDefenceWaveTimer = 0.f;

public:
	void StartFinalDefenceTimer();
	void StartFinalDefenceWave();
	void SetZombieMoveToSpot();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4GameMode.generated.h"

/**
 * - 전투는 컴포넌트로 구현 (플레이어, 좀비 체력 감소 등)
 * - 
 */
UCLASS()
class CH4_TEAMPROJECT_API ACH4GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACH4GameMode();

protected:
	virtual void StartPlay() override;
	void EndGame(EGamePhase GP);
	
public:
    // 플레이어 관리
    void OnPlayerDowned(ACH4PlayerState* PlayerState);   // 다운 발생 시
    void OnPlayerRevived(ACH4PlayerState* PlayerState);  // 소생 시

    // 승패 판정
    void CheckCondition();
	
    // 웨이브 제어 -> 웨이브 매니저?
    // void StartFinalDefenseWave();
    // void OnWaveCleared();
	
private:
	void UpdateMainServerTime() const;
	// void StartFinalDefenseTimer() const;

	FTimerHandle ServerTimeTimerHandle;
};

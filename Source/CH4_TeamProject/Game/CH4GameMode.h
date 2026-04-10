
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4_TeamProject/Game/CH4PlayerState.h"
#include "CH4GameMode.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACH4GameMode();

protected:
	virtual void StartPlay() override;
	void EndGame(EGamePhase GP) const;
	
public:
    // 플레이어 관리
    void OnPlayerDowned(ACH4PlayerState* PlayerState);   // 다운
    void OnPlayerRevived(ACH4PlayerState* PlayerState);  // 소생

    // 승패 판정
    void SetGameResult() const;
	
	FTimerHandle ServerTimeTimerHandle;

private:
	void UpdateMainServerTime() const;
			
	// void StartFinalDefenseTimer() const;	
		
    // 웨이브 제어 -> 웨이브 매니저?
    // void StartFinalDefenseWave();
    // void OnWaveCleared();
};

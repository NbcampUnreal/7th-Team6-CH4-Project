
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

	virtual void BeginPlay() override ;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	void PlayGame();
	void EndGame(EGamePhase GP);
	
    void OnPlayerDowned(ACH4PlayerState* PlayerState);
    void OnPlayerRevived(ACH4PlayerState* PlayerState);
	
    void SetGameResult();
	
	FTimerHandle ServerTimeTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void RequestReturnToLobby();
	
	void SetDayPhaseAtServer(EDayPhase NewPhase);
	void FinalDefenceWaveSpawn();
	
	// void StartFinalDefenceAtServer();
	
private:
	bool bGameStarted = false;
	bool bIsReturningToLobby = false;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4GameState.generated.h"


UCLASS()
class CH4_TEAMPROJECT_API ACH4GameState : public AGameState
{
	GENERATED_BODY()

public:
	ACH4GameState();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Leve2")
	int32 MaxLevels;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// INLINE : 함수 호출 시 코드 복붙
	FORCEINLINE float GetServerTime() const { return ServerTime(); }
	FORCEINLINE void SetServerTime(const float NewTime) { ServerTime = NewTime; }

public:	
	UPROPERTY(Replicated)
	float PhaseRemainingTime;
	
	UPROPERTY(ReplicatedUsing = OnRep_GearParts)
	int32 GearPartsCollected;
	
	UPROPERTY(Replicated)
	int32 AlivePlayerCount = 4;
	
	UPROPERTY(Replicated)
	float ServerTime = 0.f;
	
	UPROPERTY(Replicated)
	float FinalDefenceTime = 5.f * 60.f;
	
	UPROPERTY(ReplicatedUsing = OnRep_GamePhase)
	EGamePhase GamePhase;
	
	int32 GearPartsCount;
	int32 TotalGearPartsCount;
	
	UFUNCTION()
	void OnRep_CurrentPhase();
	
	UFUNCTION()
	void OnRep_GearParts();
	
	UFUNCTION()
	void OnRep_GamePhase();

	// 함수 끝의 const 의미 : 이 함수는 멤버 변수를 바꾸지 않는다.
	int32 GetAlivePlayerCount() const{ return AlivePlayerCount; }
	void AddAlivePlayerCount() { AlivePlayerCount++; }
	void SubtractAlivePlayerCount() { AlivePlayerCount--; }
	void AddGearPartsCount() { GearPartsCollected++; }
	
	bool CheckAlivePlayerIsZero(); 
	
	void SetGamePhase(EGamePhase NewPhase);
	
	float GetFinalDefenceTime() const { return FinalDefenceTime; }
	
};

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
	
	FORCEINLINE float GetServerTime() const { return ServerTime; }
	FORCEINLINE void SetServerTime(const float NewTime) { ServerTime = NewTime; }

public:	
	UPROPERTY(Replicated)
	int32 AlivePlayerCount = 4;
	
	UPROPERTY(Replicated)
	float ServerTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_GamePhase)
	EGamePhase GamePhase;
	
	UPROPERTY(ReplicatedUsing = OnRep_GearPartsCount)
	int32 GearPartsCount;
	
	UFUNCTION()
	void OnRep_GearPartsCount();
	
	UFUNCTION()
	void OnRep_GamePhase();
	
	int32 GetAlivePlayerCount() const{ return AlivePlayerCount; }
	void AddAlivePlayerCount() { AlivePlayerCount++; }
	void SubtractAlivePlayerCount() { AlivePlayerCount--; }
	
	void AddGearPartsCount() { GearPartsCount++; }
	
	bool CheckAlivePlayerIsZero();
	
	void SetGamePhase(EGamePhase NewPhase);
	
	// float GetFinalDefenceTime() const { return FinalDefenceTime; }
	// 좀비들 정보를 저장하는 구조체를 만들어서 좀비 체력 감소도 기록할 필요가 있을 듯.
	
	// UPROPERTY(Replicated)
	// float FinalDefenceTime = 5.f * 60.f;
	
	// UPROPERTY(Replicated)
	// float PhaseRemainingTime;
	
};

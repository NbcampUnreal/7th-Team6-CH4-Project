
#pragma once

#include "CoreMinimal.h"
#include "CH4GameMode.h"
#include "GameFramework/GameState.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4GameState.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4GameState : public AGameState
{
	GENERATED_BODY()

public:
	ACH4GameState();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

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
	int32 AlivePlayerCount = 0;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	float ServerTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_GamePhase, BlueprintReadWrite)
	EGamePhase GamePhase;
	
	UPROPERTY(ReplicatedUsing = OnRep_GearPartsCount)
	int32 GearPartsCount;
	
	UPROPERTY(ReplicatedUsing = OnRep_DayPhase)
	EDayPhase DayPhase = EDayPhase::None;
	
	// UPROPERTY(Replicated)
	// float DayPhaseTimeCount;

	UPROPERTY()
	class ADirectionalLight* DirectionalLight;
	
	UPROPERTY()
	class ASkyLight* SkyLight;
	
	UPROPERTY()
	class AExponentialHeightFog* Fog;
	
public:
	UFUNCTION()
	void OnRep_GearPartsCount();
	
	UFUNCTION()
	void OnRep_GamePhase();
	
	UFUNCTION()
	void OnRep_DayPhase();
	
	int32 GetAlivePlayerCount() const{ return AlivePlayerCount; }
	
	UFUNCTION(Server, Reliable)
	void AddAlivePlayerCount();
	
	UFUNCTION(Server, Reliable)
	void SubtractAlivePlayerCount();
	
	UFUNCTION()
	void AddGearPartsCount();
	
	bool CheckAlivePlayerIsZero();
	
	void ApplyDayPhaseChanges();
	
public:	
	EGamePhase GetGamePhase() const {  return GamePhase; }
	void SetGamePhase(EGamePhase NewPhase);
	
	EDayPhase GetDayPhase() const {  return DayPhase; }
	void SetDayPhase(EDayPhase NewPhase);
	
	void SetLightsAndFogActor();
	
protected:
	UPROPERTY()
	class ADirectionalLight* SunLight;
};

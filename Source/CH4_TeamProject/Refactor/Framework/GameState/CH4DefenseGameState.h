/*
 * [Refactor] ACH4DefenseGameState
 *
 * 출처: Game/CH4GameState.h/cpp 의 ACH4GameState
 * 변경:
 *   - 클래스명 ACH4GameState -> ACH4DefenseGameState (방어전 모드 전용 GameState 로 도메인 명시)
 *   - 폴더 위치 Game/ -> Refactor/Framework/GameState/
 *   - 부모 AGameState -> AGameStateBase
 *     (Match state machine 미사용. ACH4DefenseGameMode(AGameModeBase) 와 짝 통일)
 *   - 식별자 통일 Defence(영국식) -> Defense(미국식)
 *     (FinalDefenceTimerHandle -> FinalDefenseTimerHandle,
 *      FinalDefenceElapsedTime -> FinalDefenseElapsedTime,
 *      FinalDefenceDayPhase -> FinalDefenseDayPhase,
 *      UpdateFinalDefenceTimerHandle -> UpdateFinalDefenseTimerHandle,
 *      StartFinalDefenceWave -> StartFinalDefenseWave)
 *   - GameMode 캐스트를 ACH4GameMode -> ACH4DefenseGameMode 로 좁힘
 *   - PlayerController 캐스트를 ACH4PlayerController -> ACH4DefensePlayerController 로 좁힘
 *   - GameInstance 캐스트를 UCH4GameInstance -> UCH4DefenseGameInstance 로 좁힘
 *   - 사용처 없는 멤버 제거: RecentPhase (private 선언만 있고 read/write 없음)
 *   - 사용처 없는 멤버 제거: FinalDefenseDayPhase (어디에서도 read 안 함)
 *   - UPROPERTY 오타 정정: VisibleAnyWhere -> VisibleAnywhere, Category = "Leve2" -> "Level"
 *   - 사용처 없는 include 정리 (InterchangeResult.h, ZombieSpawnPoint.h, MonsterAIController.h, FinalDefenceManager.h)
 *   - GetFirstPlayerController 사용 패턴은 동작 동등성을 위해 유지.
 *     PlayerControllerIterator 전환은 후속 작업
 *   - Tick 오버라이드는 본문이 Super 호출만 하던 무의미 오버라이드여서 제거 + bCanEverTick=false 유지
 * 사유: 도메인 명확화 + 폴더 정리 + 베이스 통일 + Defense* 트리 식별자/타입 정합
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4DefenseGameState.generated.h"

class ADirectionalLight;
class ASkyLight;
class AExponentialHeightFog;
class USoundBase;

UCLASS()
class CH4_TEAMPROJECT_API ACH4DefenseGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACH4DefenseGameState();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score = 0;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);

	FORCEINLINE float GetServerTime() const { return ServerTime; }
	FORCEINLINE void SetServerTime(const float NewTime) { ServerTime = NewTime; }

	UPROPERTY(BlueprintReadOnly, Category = "DayPhase")
	int32 DayCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "DayPhase")
	float TotalDayPhaseCycleTime = 7.f * 60.f;

	UPROPERTY(BlueprintReadOnly, Category = "DayPhase")
	float DayTime = 2.5f * 60.f;

	UPROPERTY(BlueprintReadOnly, Category = "DayPhase")
	float EveningTime = 0.5f * 60.f;

	UPROPERTY(BlueprintReadOnly, Category = "DayPhase")
	float NightTime = 4.f * 60.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Players")
	int32 AlivePlayerCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ServerTime, BlueprintReadOnly, Category = "Time")
	float ServerTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_GamePhase, BlueprintReadWrite, Category = "Phase")
	EGamePhase GamePhase = EGamePhase::None;

	UPROPERTY(ReplicatedUsing = OnRep_GearPartsCount, BlueprintReadOnly, Category = "Phase")
	int32 GearPartsCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_DayPhase, BlueprintReadOnly, Category = "DayPhase")
	EDayPhase DayPhase = EDayPhase::None;

	UPROPERTY()
	TObjectPtr<ADirectionalLight> DirectionalLight;

	UPROPERTY()
	TObjectPtr<ASkyLight> SkyLight;

	UPROPERTY()
	TObjectPtr<AExponentialHeightFog> Fog;

	UFUNCTION()
	void OnRep_GearPartsCount();

	UFUNCTION()
	void OnRep_GamePhase();

	UFUNCTION()
	void OnRep_DayPhase();

	UFUNCTION()
	void OnRep_ServerTime();

	void UpdateLapsedTime();

	int32 GetAlivePlayerCount() const { return AlivePlayerCount; }

	UFUNCTION(Server, Reliable)
	void AddAlivePlayerCount();

	UFUNCTION(Server, Reliable)
	void SubtractAlivePlayerCount();

	UFUNCTION()
	void AddGearPartsCount();

	bool CheckAlivePlayerIsZero();

	void ApplyDayPhaseChanges(EDayPhase DP);
	void FindLightAndFog();

	UFUNCTION()
	void PlayZombieSound();

	EGamePhase GetGamePhase() const { return GamePhase; }
	void SetGamePhase(EGamePhase NewPhase);

	EDayPhase GetDayPhase() const { return DayPhase; }
	void SetDayPhase(EDayPhase NewPhase);

	void UpdateFinalDefenseTimerHandle();
	void StartFinalDefenseWave();

private:
	FTimerHandle ServerTimeHandle;
	FTimerHandle FinalDefenseTimerHandle;
	int32 FinalDefenseElapsedTime = 0;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> ZombieSound;
};

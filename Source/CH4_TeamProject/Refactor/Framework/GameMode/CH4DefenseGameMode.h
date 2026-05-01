/*
 * [Refactor] ACH4DefenseGameMode
 *
 * 출처:
 *   - Game/CH4GameMode.h/cpp 의 ACH4GameMode (방어전 모드 게임 루프 전체)
 *   - Game/GameManagers/FinalDefenceManager.h/cpp 의 AFinalDefenceManager (최종 방어 Wave Timer)
 * 변경:
 *   - 두 클래스를 ACH4DefenseGameMode 로 통합
 *     (도메인 명확화 + Manager 안티패턴 제거)
 *   - 부모 AGameMode -> AGameModeBase (Match state machine 미사용)
 *   - 폴더 위치 Game/, Game/GameManagers/ -> Refactor/Framework/GameMode/
 *   - 식별자 통일 Defence(영국식) -> Defense(미국식)
 *   - 의존 타입을 모두 Refactor/ 의 리팩토링 클래스로 전환:
 *       ACH4PlayerController       -> ACH4DefensePlayerController
 *       ACH4GameState              -> ACH4DefenseGameState
 *       ACH4PlayerState            -> ACH4DefensePlayerState
 *       ACH4Character              -> ACH4PlayerCharacter        (DefaultPawnClass)
 *       AItemSpawnPoint            -> ACH4ItemSpawnPoint
 *       AZombieSpawnPoint          -> ACH4ZombieSpawnPoint
 *   - ZombieSpawn 의 매직 숫자(5,10,4,6 / 1,3,1,2 / 4,8,2,6 / 7,15,6,10) 제거.
 *     ACH4ZombieSpawnPoint 가 UCH4ZombieSpawnData 기반 데이터 주도형으로 바뀌어,
 *     단계별 Wave 데이터 자산을 UPROPERTY(EditDefaultsOnly) 로 노출
 *     (StageStartWaveData, DayWaveData, NightWaveData, FinalWaveData).
 *     자산이 nullptr 이면 SpawnPoint 자체가 보유한 SpawnData 가 사용됨
 *   - Cast 결과 nullptr 가드 누락분(BeginPlay) 보강
 *   - 원본의 미선언 멤버 참조 정정: SetDayPhaseAtServer 가 GS->ElapsedTime 을 읽고 쓰는데
 *     ACH4GameState 에는 ElapsedTime 이 선언된 적이 없음(원본은 컴파일 불가 상태로 보임).
 *     의미상 1초 단위 누적 카운터인 ServerTime 으로 정정
 *   - DefaultPawnClass 의 BP 하드코딩 경로(/Game/Player/PlayerBluePrint/BP_Player) 는
 *     동작 동등성을 위해 유지. 컷오버 시 BP 를 ACH4PlayerCharacter 부모로 reparent 하지 않으면
 *     FClassFinder 결과가 nullptr 이 되어 DefaultPawnClass 가 엔진 기본값으로 폴백됨
 * 사유: 도메인 명확화 + GameMode 그룹화 + Manager 안티패턴 제거 + 철자/식별자 통일
 *      + 리팩토링 클래스 의존 통일
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4DefenseGameMode.generated.h"

class ACH4DefensePlayerState;
class UCH4ZombieSpawnData;

UCLASS()
class CH4_TEAMPROJECT_API ACH4DefenseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACH4DefenseGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void PlayGame();
	void EndGame(EGamePhase GP);

	void OnPlayerDowned(ACH4DefensePlayerState* PlayerState);
	void OnPlayerRevived(ACH4DefensePlayerState* PlayerState);

	void SetGameResult();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void RequestReturnToLobby();

	void SetDayPhaseAtServer(EDayPhase NewPhase);
	void FinalDefenseWaveSpawn();

	void StartDefenseTimer();

	FTimerHandle ServerTimeTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	TObjectPtr<UCH4ZombieSpawnData> StageStartWaveData;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	TObjectPtr<UCH4ZombieSpawnData> DayWaveData;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	TObjectPtr<UCH4ZombieSpawnData> NightWaveData;

	UPROPERTY(EditDefaultsOnly, Category = "Wave")
	TObjectPtr<UCH4ZombieSpawnData> FinalWaveData;

private:
	bool bGameStarted = false;
	bool bIsReturningToLobby = false;
	float DefenseWaveTimer = 0.0f;
};

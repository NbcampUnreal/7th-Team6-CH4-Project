/*
 * [Refactor] ANewCH4LobbyGameMode
 *
 * 출처: Title and Lobby/CH4LobbyGameMode.h/cpp 의 ACH4LobbyGameMode
 * 변경:
 *   - 클래스명 ACH4LobbyGameMode -> ANewCH4LobbyGameMode (원본과의 컷오버 충돌 회피용 임시명).
 *     컷오버 시 ACH4LobbyGameMode 로 환원
 *   - 폴더 위치 "Title and Lobby" -> Refactor/Framework/GameMode/ (공백 폴더 금지 + GameMode 그룹화)
 *   - 부모 AGameMode -> AGameModeBase
 *     (Match state machine 미사용. ACH4DefenseGameMode 와 동일한 경량 베이스)
 *   - PlayerControllerClass = ACH4LobbyPlayerController (로비 전용 PC 와 결합)
 *   - GameStateClass = ACH4GameState, PlayerStateClass = ACH4PlayerState 명시 (BP 오버라이드 가능).
 *     로비 도메인용 리팩토링 클래스(ACH4LobbyGameState/ACH4LobbyPlayerState) 가 아직 없어
 *     원본을 그대로 참조한다. 로비 PlayerState 의 LifeState/SetPlayerLifeState 사용 패턴은
 *     방어전용 ACH4DefensePlayerState 와 동일하므로, 후속 리팩토링에서 공통 베이스를
 *     추출하거나 도메인별 PlayerState 를 신설할 때까지 본 의존을 유지
 *   - SetPlayerLifeState 파라미터를 ACH4PlayerController -> ACH4LobbyPlayerController 로 좁힘
 *   - 생성자 접근 제한자를 public 으로 정정 (원본은 private 이지만 BP/엔진 스폰을 위해 public 이어야 함)
 *   - 임시 로그(LogTemp Warning/Error)는 동작 동등성을 위해 유지. 컷오버 후 영역별 로그 카테고리로 정리
 *   - BeginPlay 오버라이드는 본문이 Super 호출만 하던 무의미 오버라이드여서 제거
 * 사유: 폴더 공백 제거 + GameMode 그룹화 + ACH4DefenseGameMode 와 동일한 베이스/PC 결합 패턴 통일
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NewCH4LobbyGameMode.generated.h"

class ACH4LobbyPlayerController;

UCLASS()
class CH4_TEAMPROJECT_API ANewCH4LobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANewCH4LobbyGameMode();

	UFUNCTION(BlueprintCallable)
	void SetPlayerLifeState(ACH4LobbyPlayerController* PC);

	UFUNCTION(BlueprintCallable)
	void SetReadyPlayerCount();

	UPROPERTY()
	int32 ReadyPlayerCount = 0;

	UPROPERTY()
	int32 MaxPlayerCount = 2;
};

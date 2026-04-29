/*
 * [Refactor] ACH4TitleGameMode
 *
 * 출처: Title and Lobby/CH4Title.h/cpp 의 ACH4Title
 * 변경:
 *   - 클래스명 ACH4Title -> ACH4TitleGameMode
 *     (도메인 명확화: GameMode 파생임을 이름에 명시. 형제 ACH4LobbyGameMode/ACH4DefenseGameMode 와 형식 통일)
 *   - 폴더 위치 "Title and Lobby" -> Refactor/Framework/GameMode/ (공백 폴더 금지 + GameMode 그룹화)
 *   - 부모 ACH4GameMode -> AGameModeBase
 *     (Match state machine 미사용. 형제 GameMode 들과 동일한 경량 베이스로 통일)
 *   - PlayerControllerClass = ACH4TitlePlayerController (타이틀 전용 PC 와 결합)
 *   - 본체 동등 (생성자, EnjoyToServerLobby)
 *   - GetFirstPlayerController 사용 패턴은 동작 동등성을 위해 유지. 멀티플레이 정정은
 *     별도 작업으로 분리
 *   - 하드코딩 IP "43.200.5.254:7777" 도 동작 동등성을 위해 유지. UPROPERTY 노출은 후속 작업
 * 사유: 도메인 명확화 + 폴더 공백 제거 + GameMode 그룹화 + 형제 GameMode 와 베이스/PC 결합 패턴 통일
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CH4TitleGameMode.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4TitleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACH4TitleGameMode();

	UFUNCTION(BlueprintCallable)
	void EnjoyToServerLobby();
};

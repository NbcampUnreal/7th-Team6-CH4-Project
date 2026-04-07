
#include "CH4GameMode.h"

#include "CH4_TeamProject/Player/CH4Character.h"
#include "UObject/ConstructorHelpers.h"
#include "CH4_TeamProject/Game/CH4GameState.h"
#include "CH4_TeamProject/Game/CH4PlayerState.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"


ACH4GameMode::ACH4GameMode()
{
	GameStateClass = ACH4GameState::StaticClass();
	PlayerStateClass = ACH4PlayerState::StaticClass();
	PlayerControllerClass = ACH4PlayerController::StaticClass();

	GamePhase = EGamePhase::StartStage;
	
	static ConstructorHelpers::FClassFinder<ACH4Character>
		PlayerCharacter(TEXT("Game/Player/PlayerBluePrint/BP_Player.BP_Player_C"));

	if (PlayerCharacter.Class)
	{
		DefaultPawnClass = PlayerCharacter.Class;
	}
}

void ACH4GameMode::StartPlay()
{
	Super::StartPlay();
	
	// 서버 시간 갱신
	GetWorldTimerManager().SetTimer(
		ServerTimeTimerHandle, 
		this, 
		&ACH4GameMode::UpdateServerTime, 
		1.f, 
		true, 
		0.f);
}

void ACH4GameMode::SetGamePhase(EGamePhase NewPhase)
{
	GamePhase = NewPhase;
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->OnRep_GamePhase();
	}
}

void ACH4GameMode::OnPlayerDowned(ACH4PlayerState* PlayerState)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Downed"));
	
	// 객체를 호출해야 어떤 유저의 플레이어 스테이트인지 구분 가능
	ACH4PlayerState* PS = Cast<ACH4PlayerState>(PlayerState);
	if (PS)
	{
		PS->SetLifeState(EPlayerLifeState::Downed);;
	}
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->SubtractAlivePlayerCount();
	}
	
	if (GS->AlivePlayerCount <= 0)
	{
		GS->AlivePlayerCount = 0;
		
	}
	
	// 다운 애니메이션 재생 명령(Client RPC)
	// 움직이지 못하도록
}

void ACH4GameMode::OnPlayerRevived(ACH4PlayerState* PlayerState)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Revived"));
	
	ACH4PlayerState* PS = Cast<ACH4PlayerState>(PlayerState);
	if (PS)
	{
		PS->LifeState = EPlayerLifeState::Alive;
	}
		
	// <플레이어 소생>
	// 조건 : 팀원이 다운된 플레이어 주변에서(트리거) 소생 아이템 사용 시 
	// 움직일 수 있도록(사망 시 아이템 드롭 x)
}

void ACH4GameMode::CheckWinCondition()
{
	// 디펜스 타이머 <= 0  && AliveCount >= 1
	// 스테이지 클리어
}

void ACH4GameMode::CheckLoseCondition()
{
	// AliveCount <= 0 이면 패배
	// 게임 종료 함수 호출
}

void ACH4GameMode::OnWaveCleared()
{
	// 웨이브 클리어
}

void ACH4GameMode::UpdateServerTime() const
{
	if (ACH4GameState* GS = GetGameState<ACH4GameState>())
	{
		// 월드 타이머 호출
		GS->SetServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ACH4GameMode::StartFinalDefenseTimer() const
{
	// 구조 신호 발생 -> 디펜스 타이머 시작 (DataBase::DefenceTimer)
}

void ACH4GameMode::EndGame()
{
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS && GS->AlivePlayerCount <= 0)
	{
		GS->AlivePlayerCount = 0;
		EndGame(EGameResult::Lose)
	}
	
	if (GS && )
	
	
}

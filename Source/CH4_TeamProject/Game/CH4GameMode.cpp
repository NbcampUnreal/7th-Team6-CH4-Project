
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
	
	static ConstructorHelpers::FClassFinder<ACH4Character>
		PlayerCharacter(TEXT("Game/Player/PlayerBluePrint/BP_Player.BP_Player_C"));

	if (PlayerCharacter.Class)
	{
		DefaultPawnClass = PlayerCharacter.Class;
	}
	ACH4GameMode::StartPlay();
}

void ACH4GameMode::StartPlay()
{
	Super::StartPlay();
	
	// 서버 시간 갱신
	GetWorldTimerManager().SetTimer(
		ServerTimeTimerHandle, 
		this, 
		&ACH4GameMode::UpdateMainServerTime, 
		1.f, 
		true, 
		0.f);
}

void ACH4GameMode::EndGame(EGamePhase GP) const
{
	// UI 변경 명령(컨트롤러) / 결과 표시 / 맵 이동 준비
	
	// 모든 플레이어 입력 막기 (서버 기준) -> PlayerController에 위임?
	// 현재 월드에 존재하는 모든 PlayerController를 순회(반복)하기 위한 반복자(iterator)
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			PC->SetIgnoreMoveInput(true);
			PC->SetIgnoreLookInput(true);
			// PC->SetIgnoreJumpInput(true); -> 점프 인풋 완성되면 주석 해제
		}
	}
	
	// 타이머 정지
	GetWorldTimerManager().ClearAllTimersForObject(this);
	
	if (GP == EGamePhase::Clear)
	{
		// 클리어 UI 명령
	}
	else if (GP == EGamePhase::Lose)
	{
		// 패배 UI 명령
	}
}

void ACH4GameMode::OnPlayerDowned(ACH4PlayerState* PlayerState)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Downed"));
	
	ACH4PlayerState* PS = Cast<ACH4PlayerState>(PlayerState);
	if (PS)
	{
		PS->SetLifeState(EPlayerLifeState::Downed);
	}
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->SubtractAlivePlayerCount();
	}
	
	if (GS->AlivePlayerCount <= 0)
	{
		GS->AlivePlayerCount = 0;
		
		EndGame(GS->GamePhase);
	}
	else
	{
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(PS->GetOwner()); // 해당 컨트롤러를 소유한 객체를 ()에서 불러오기
		if (PC)
		{
			PC->Client_HandlePlayerDowned();
		}
	}
}

void ACH4GameMode::OnPlayerRevived(ACH4PlayerState* PlayerState)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Revived"));
	
	ACH4PlayerState* PS = Cast<ACH4PlayerState>(PlayerState);
	if (PS)
	{
		PS->SetLifeState(EPlayerLifeState::Alive);
	}
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->AddAlivePlayerCount();
	}
	
	ACH4PlayerController* PC = Cast<ACH4PlayerController>(PS->GetOwner());
	if (PC)
	{
		PC->Client_HandlePlayerRevived();
	}
	// 소생 조건 : 팀원이 다운된 플레이어에 상호작용 키 입력 시 -> 이것도 컨트롤러에서 구현?
}

void ACH4GameMode::CheckCondition() // 플레이어가 다운될 때마다 체크
{
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS && GS->AlivePlayerCount <= 0)
	{
		EndGame(EGamePhase::Lose);
	}
	else if (GS && GS->AlivePlayerCount >= 1 && GS->GearPartsCount == 3)
	{
		// 스테이지 클리어 조건 : 발전기 부품 다 모으면으로(MVP)
		EndGame(EGamePhase::Clear);
	}
}

// void ACH4GameMode::StartFinalDefenseWave()
// {
// 	웨이브 시작 명령
// }

// void ACH4GameMode::OnWaveCleared()
// {
// 	웨이브 중지, 플레이어컨트롤러 입력x, UI 변경 명령 
// }

void ACH4GameMode::UpdateMainServerTime() const
{
	if (ACH4GameState* GS = GetGameState<ACH4GameState>())
	{
		// 월드 타이머 호출
		GS->SetServerTime(GetWorld()->GetTimeSeconds());
	}
}

// void ACH4GameMode::StartFinalDefenseTimer() const
// {
// 	// 구조 신호 발생 -> 디펜스 타이머 시작 (DataBase::DefenceTimer)
// }

void ACH4GameMode::MoveToLobby() const
{
	// 로비 UI로 이동하는 로직?
}


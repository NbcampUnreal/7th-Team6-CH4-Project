
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
}

void ACH4GameMode::StartPlay() override
{
	Super::StartPlay();
	
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
	// GameState에 반영
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->SetGamePhase(GP);
	}
	
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
	
	PlayerState->SetLifeState(EPlayerLifeState::Downed);
	
	/* PlayerState를 매개변수로 받아오고 있기 때문에 불러오기 필요 없음(확인받은 후 삭제)
	 *ACH4PlayerState* PS = Cast<ACH4PlayerState>(PlayerState);
	if (PS)
	{
		PS->SetLifeState(EPlayerLifeState::Downed);
	}
	*/
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->SubtractAlivePlayerCount();
	}
	
	if (GS->AlivePlayerCount > 0)
	{
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(PlayerState->GetOwner()); // 해당 컨트롤러를 소유한 객체를 ()에서 불러오기
		if (PC)
		{
			PC->Client_HandlePlayerDowned();
		}
	}
	else // 모든 플레이어 다운 시
	{
		CheckCondition();
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
}

// 플레이어가 다운될 때마다 체크
void ACH4GameMode::CheckCondition() 
{
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS && GS->AlivePlayerCount <= 0)
	{
		EndGame(EGamePhase::Lose);
	}
	else if (GS && GS->AlivePlayerCount > 0 && GS->GearPartsCount == 3)
	{
		// 클리어 조건 : 발전기 부품 다 모으면으로(MVP)
		EndGame(EGamePhase::Clear);
	}
}

void ACH4GameMode::MoveToLobby() const
{
	// 로비 UI로 이동하는 로직?
}

void ACH4GameMode::UpdateMainServerTime() const
{
	if (ACH4GameState* GS = GetGameState<ACH4GameState>())
	{
		// 월드 타이머 호출
		GS->SetServerTime(GetWorld()->GetTimeSeconds());
	}
}

// --------------------------------------------------
// void ACH4GameMode::StartFinalDefenseTimer() const
// {
// 	// 구조 신호 발생 -> 디펜스 타이머 시작 (DataBase::DefenceTimer)
// }

// void ACH4GameMode::StartFinalDefenseWave()
// {
// 	웨이브 시작 명령
// }

// void ACH4GameMode::OnWaveCleared()
// {
// 	웨이브 중지, 플레이어컨트롤러 입력x, UI 변경 명령 
// }


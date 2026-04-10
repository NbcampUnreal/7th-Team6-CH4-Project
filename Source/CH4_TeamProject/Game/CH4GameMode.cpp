
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

void ACH4GameMode::StartPlay()
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
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(It->Get());
		if (PC)
		{
			PC->Client_DisablePlayerInput();
		}
		
		if (GP == EGamePhase::Clear)
		{
			PC->Client_InvokeGameClearUI();
		}
		else if (GP == EGamePhase::Lose)
		{
			PC->Client_InvokeGameLoseUI();
		}
	}
	
	GetWorldTimerManager().ClearAllTimersForObject(this); // 타이머 정지
}

void ACH4GameMode::OnPlayerDowned(ACH4PlayerState* PlayerState)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Downed"));
	
	if (PlayerState->PlayerReviveCount <= 0) // 데카 0
	{
		PlayerState->SetLifeState(EPlayerLifeState::Dead);
		return;
	}
	
	PlayerState->SetLifeState(EPlayerLifeState::Downed); // 플레이어 상태 변경
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->SubtractAlivePlayerCount(); // 생존자 수 감소
	}
	
	if (GS->AlivePlayerCount > 0)
	{
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(PlayerState->GetOwner()); // 해당 컨트롤러를 소유한 객체를 ()에서 불러오기
		if (PC)
		{
			PC->Client_DisablePlayerInput();
			PC->Client_PlayDownAnim();
			PC->Client_InvokeDownUI();
		}
	}
	else // 모든 플레이어 다운 시
	{
		SetGameResult();
	}
}

void ACH4GameMode::OnPlayerRevived(ACH4PlayerState* PlayerState)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Revived"));
	
	if (PlayerState->PlayerReviveCount <= 0)
		return;

	PlayerState->PlayerReviveCount--;
	PlayerState->SetLifeState(EPlayerLifeState::Alive);
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->AddAlivePlayerCount();
	}
	
	ACH4PlayerController* PC = Cast<ACH4PlayerController>(PlayerState->GetPlayerController());
	if (PC)
	{
		PC->Client_EnablePlayerInput();
		PC->Client_PlayReviveAnim();
		PC->Client_HideDownUI();
	}
}

void ACH4GameMode::SetGameResult() const
{
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS && GS->AlivePlayerCount <= 0)
	{
		EndGame(EGamePhase::Lose);
	}
	else if (GS && GS->AlivePlayerCount > 0 && GS->GearPartsCount == 3)
	{
		EndGame(EGamePhase::Clear);
	}
	
	ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetWorld()->GetGameState());
	if (PC)
	{
		PC->Client_MoveToLobby();
	}
}

void ACH4GameMode::UpdateMainServerTime() const
{
	if (ACH4GameState* GS = GetGameState<ACH4GameState>())
	{
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


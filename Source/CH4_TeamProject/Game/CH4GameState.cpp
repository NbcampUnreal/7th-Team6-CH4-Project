
#include "CH4GameState.h"
#include "CH4GameInstance.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"
#include "Net/UnrealNetwork.h"

ACH4GameState::ACH4GameState()
{
	bReplicates = true;
	
	GamePhase = EGamePhase::StartStage;
	GearPartsCount = 0;
	
	// 기본값 초기화 (예: Score 초기화)
	Score = 0;
	// LevelDuration = 60.0f;
	MaxLevels = 2;
}

void ACH4GameState::AddScore(int32 Amount)
{
	Score += Amount;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UCH4GameInstance* CH4GameInstance = Cast<UCH4GameInstance>(GameInstance);
		if (CH4GameInstance)
		{
			CH4GameInstance->AddToScore(Amount);
		}
	}
}

int32 ACH4GameState::GetScore() const
{
	return Score; // 혹은 저장된 변수 반환
}

void ACH4GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACH4GameState, ServerTime);
	DOREPLIFETIME(ACH4GameState, GamePhase);
	DOREPLIFETIME(ACH4GameState, GearPartsCount);
	DOREPLIFETIME(ACH4GameState, AlivePlayerCount);
}

void ACH4GameState::OnRep_GearPartsCount()
{
	UE_LOG(LogTemp, Warning, TEXT("GearParts Count: %d"), GearPartsCount);
	// UI : Collected GearParts 갱신 -> 컨트롤러
}

void ACH4GameState::OnRep_GamePhase() // 변경 시 자동 호출
{
	UE_LOG(LogTemp, Warning, TEXT("GamePhase Changed: %d"), GamePhase);
	
	ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC) return;
	
	if (GamePhase == EGamePhase::Clear)
	{
		PC->ShowGameClear();
	}
	else if (GamePhase == EGamePhase::Lose)
	{
		PC->ShowGameOver();
	}
	else if (GamePhase == EGamePhase::StartStage)
	{
		PC->StartGame();
	}
}

void ACH4GameState::AddAlivePlayerCount_Implementation()
{
	AlivePlayerCount++;
}

void ACH4GameState::SubtractAlivePlayerCount_Implementation()
{
	AlivePlayerCount--;
}

void ACH4GameState::AddGearPartsCount()
{
	GearPartsCount++;
	if (GearPartsCount >= 3)
	{
		if (HasAuthority())
		{
			ACH4GameMode* GM = Cast<ACH4GameMode>(GetWorld()->GetAuthGameMode());
			GM->SetGameResult();
		}
	}; 
}

bool ACH4GameState::CheckAlivePlayerIsZero()
{
	bool AlivePlayerIsZero;
	if (AlivePlayerCount > 0)
	{
		AlivePlayerIsZero = false;
	}
	else
	{
		AlivePlayerIsZero = true;
		SetGamePhase(EGamePhase::Lose);
		
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetWorld()->GetFirstPlayerController());
		PC->Client_InvokeGameLoseUI();
	}
	return AlivePlayerIsZero;
}

void ACH4GameState::SetGamePhase(EGamePhase NewPhase)
{
	if (GamePhase == NewPhase) return;
	GamePhase = NewPhase;
}

// void ACH4GameState::OnRep_CurrentPhase()
// {
// 	UE_LOG(LogTemp, Warning, TEXT("Current Phase: %hhd"), GamePhase);
// 	// UI : GamePhase 갱신
// }

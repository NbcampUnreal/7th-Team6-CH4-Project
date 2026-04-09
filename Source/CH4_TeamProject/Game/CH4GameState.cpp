
#include "CH4GameState.h"
#include "CH4GameInstance.h"
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

void ACH4GameState::OnRep_GamePhase()
{
	UE_LOG(LogTemp, Warning, TEXT("GamePhase Changed: %d"), GamePhase);
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

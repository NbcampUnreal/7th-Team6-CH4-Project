// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4GameState.h"
#include "CH4GameInstance.h"
#include "Net/UnrealNetwork.h"

ACH4GameState::ACH4GameState()
{
	bReplicates = true;
	
	GamePhase = EGamePhase::StartStage;
	GearPartsCount = 0;
	TotalGearPartsCount = 3;
	
	// 기본값 초기화 (예: Score 초기화)
	Score = 0;
	LevelDuration = 60.0f;
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
	DOREPLIFETIME(ACH4GameState, CurrentPhase);
	DOREPLIFETIME(ACH4GameState, PhaseRemainingTime);
	DOREPLIFETIME(ACH4GameState, GearPartsCollected);
}

// 변수 복제 시 UI 갱신
void ACH4GameState::OnRep_CurrentPhase()
{
	UE_LOG(LogTemp, Warning, TEXT("Current Phase: %hhd"), GamePhase);
	// UI : GamePhase 갱신
}

void ACH4GameState::OnRep_GearParts()
{
	UE_LOG(LogTemp, Warning, TEXT("GearParts Count: %d/%d"), GearPartsCount, TotalGearPartsCount);
	// UI : Collected GearParts 갱신
}

void ACH4GameState::OnRep_GamePhase()
{
	UE_LOG(LogTemp, Warning, TEXT("GamePhase Changed: %d"), GamePhase);

	switch (GamePhase)
	{
	case EGamePhase::StartStage :
		// UI : 라운드 시작 알림
		break;
		
	case EGamePhase::FinalDefense :
		// UI : 디펜스 시작 메세지
		break;
		
		case EGamePhase::Clear :
		// UI : 스테이지 클리어
		break;
	}
}

void ACH4GameState::OnRep_GameResult()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Result Changed"));
	
	switch (GameResult)
	{
	case EGameResult::None:
		break;
		
	case EGameResult::Win:
		// 승리 UI
		break;

	case EGameResult::Lose:
		// 패배 UI
		break;
	}
}

void ACH4GameState::SetGameResult(EGameResult NewResult)
{
	if (GameResult == NewResult) return;
	
	GameResult = NewResult;
	OnRep_GameResult(); // 서버에서도 실행

}



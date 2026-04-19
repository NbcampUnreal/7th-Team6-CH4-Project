// Fill out your copyright notice in the Description page of Project Settings.


#include "DayPhaseManager.h"

#include "CH4_TeamProject/Game/CH4GameState.h"
#include "Net/UnrealNetwork.h"

ADayPhaseManager::ADayPhaseManager()
{
	DayTime = 7.f * 60.f;
	EveningTime = 4.5f * 60.f;
	NightTime = 4.f * 60.f;
	
	MaxDayPhaseTime = DayTime; // 7분 (2.5 / 0.5 / 4)
	RemainingDayPhaseTime = MaxDayPhaseTime;
}

ADayPhaseManager::~ADayPhaseManager()
{
}

void ADayPhaseManager::BeginPlay()
{
	Super::BeginPlay();
	
	PrimaryActorTick.bCanEverTick = false;

}

void ADayPhaseManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateDayPhaseTimeCountdown_Implementation();
}

void ADayPhaseManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ADayPhaseManager, RemainingDayPhaseTime);
}

void ADayPhaseManager::OnTimerFinished()
{
	UE_LOG(LogTemp, Error, TEXT("낮밤 타이머 초기화."));
	
	RemainingDayPhaseTime = MaxDayPhaseTime;
	
	ACH4GameState* GS = GetGameState<ACH4GameState>();
	if (!GS) return;
	GS->SetDayPhase(EDayPhase::Day);
	
	StartDayPhaseTimeCountdown();
}

void ADayPhaseManager::UpdateDayPhaseTimeCountdown_Implementation()
{
	RemainingDayPhaseTime--;
	
	ACH4GameState* GS = GetGameState<ACH4GameState>();
	GS->SetDayPhase(EDayPhase::Day);
	
	if (RemainingDayPhaseTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(DayPhaseTimeCountdownHandle);
		OnTimerFinished();
	}
	
	if (GS && GS->GetGamePhase() == EGamePhase::FinalDefense)
	{
		// 게임페이즈 타이머 중지 및 파이널 디펜스 매니저의 함수 호출
	}
}

void ADayPhaseManager::StartDayPhaseTimeCountdown_Implementation()
{
	if (!HasAuthority() || !GetWorld()) return;
	
	GetWorldTimerManager().SetTimer(
		DayPhaseTimeCountdownHandle,
		this,
		&ADayPhaseManager::UpdateDayPhaseTimeCountdown,
		1.f,
		true);
}

void ADayPhaseManager::Server_SetDayPhaseByTime_Implementation()
{	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (!GS) return;
	
	if (RemainingDayPhaseTime > EveningTime && RemainingDayPhaseTime <= MaxDayPhaseTime)
	{
		GS->SetDayPhase(EDayPhase::Day);
		UE_LOG(LogTemp, Error, TEXT("낮"));
	}
	else if (RemainingDayPhaseTime <= EveningTime && RemainingDayPhaseTime > NightTime)
	{
		GS->SetDayPhase(EDayPhase::Evening);
		UE_LOG(LogTemp, Error, TEXT("해질녘"));
	}
	else if (RemainingDayPhaseTime <= NightTime && RemainingDayPhaseTime > 0)
	{
		GS->SetDayPhase(EDayPhase::Night);
		UE_LOG(LogTemp, Error, TEXT("밤"));
	}
	else
	{
		OnTimerFinished();
	}
}

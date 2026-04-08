// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4PlayerState.h"

#include "CH4GameMode.h"

ACH4PlayerState::ACH4PlayerState() : CurrentHP(MaxHP), MaxHP(100.f) // SpawnPointIndex()
{
}

void ACH4PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4PlayerState, CurrentHP);
	DOREPLIFETIME(ACH4PlayerState, MaxHP);
}

void ACH4PlayerState::OnRep_LifeState()
{
	// UI 갱신 or 캐릭터 반응
}

void ACH4PlayerState::SetLifeState(EPlayerLifeState NewState)
{
	LifeState = NewState;
	OnRep_LifeState(); // 서버에 반영
}

void ACH4PlayerState::OnRep_IsDowned()
{
}

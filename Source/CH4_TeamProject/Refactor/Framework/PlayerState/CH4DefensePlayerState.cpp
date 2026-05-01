#include "CH4DefensePlayerState.h"

#include "Net/UnrealNetwork.h"

ACH4DefensePlayerState::ACH4DefensePlayerState()
{
	bReplicates = true;
	PlayerReviveCount = 2;
}

void ACH4DefensePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4DefensePlayerState, LifeState);
	DOREPLIFETIME(ACH4DefensePlayerState, PlayerReviveCount);
}

void ACH4DefensePlayerState::OnRep_LifeState()
{
}

void ACH4DefensePlayerState::SetLifeState(EPlayerLifeState NewState)
{
	LifeState = NewState;
}

void ACH4DefensePlayerState::SetPlayerLifeState(EPlayerLifeState NewState)
{
	SetLifeState(NewState);
}

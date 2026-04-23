
#include "CH4PlayerState.h"

#include "Net/UnrealNetwork.h"
#include "CH4GameMode.h"

ACH4PlayerState::ACH4PlayerState()
		
{
	bReplicates = true;
	
	PlayerReviveCount = 2;
}

void ACH4PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4PlayerState, LifeState);
	DOREPLIFETIME(ACH4PlayerState, PlayerReviveCount);
}

void ACH4PlayerState::OnRep_LifeState()
{
}

void ACH4PlayerState::SetLifeState(EPlayerLifeState NewState)
{
	LifeState = NewState;
}

void ACH4PlayerState::SetPlayerLifeState(EPlayerLifeState PL)
{
	LifeState = PL;
}
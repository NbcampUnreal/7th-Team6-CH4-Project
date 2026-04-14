
#include "CH4PlayerState.h"

#include "Net/UnrealNetwork.h"
#include "CH4GameMode.h"

ACH4PlayerState::ACH4PlayerState()
		
{
	bReplicates = true;
	
	
	PlayerReviveCount = 0;//원래2였음
}

void ACH4PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4PlayerState, LifeState);
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
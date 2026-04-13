
#include "CH4PlayerState.h"

#include "Net/UnrealNetwork.h"
#include "CH4GameMode.h"

ACH4PlayerState::ACH4PlayerState()
		
{
	bReplicates = true;
	
	MaxHP = 100.f; 		
	CurrentHP = MaxHP;
	PlayerReviveCount = 2;
}

void ACH4PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4PlayerState, CurrentHP);
	DOREPLIFETIME(ACH4PlayerState, MaxHP);
	DOREPLIFETIME(ACH4PlayerState, LifeState);
}

void ACH4PlayerState::OnRep_LifeState()
{
}

void ACH4PlayerState::SetLifeState(EPlayerLifeState NewState)
{
	LifeState = NewState;
}

void ACH4PlayerState::Server_SetCurrentHP_Implementation(float Damage)
{
	CurrentHP -= Damage;
	if (CurrentHP <= 0)
	{
		ACH4GameMode* GM = GetWorld()->GetAuthGameMode<ACH4GameMode>();
		if (GM)
		{
			GM->OnPlayerDowned(this);
		}
	}
}

void ACH4PlayerState::SetPlayerLifeState(EPlayerLifeState PL)
{
	LifeState = PL;
}
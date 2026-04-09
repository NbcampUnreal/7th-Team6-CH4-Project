
#include "CH4PlayerState.h"

#include "Net/UnrealNetwork.h"
#include "CH4GameMode.h"

ACH4PlayerState::ACH4PlayerState() : 
		MaxHP(100.f), 		
		CurrentHP(MaxHP) // 초기화 순서에 유의
		// SpawnPointIndex()
{
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
	// UI 갱신 or 캐릭터 반응
}

void ACH4PlayerState::SetLifeState(EPlayerLifeState NewState)
{
	LifeState = NewState;
	OnRep_LifeState(); // 서버에 반영
}

void ACH4PlayerState::SetCurrentHP(float Damage)
{
	CurrentHP -= Damage;
	if (CurrentHP <= 0)
	{
		Server_RequestDown();
	}
}

void ACH4PlayerState::Server_RequestDown_Implementation()
{
	ACH4GameMode* GM = GetWorld()->GetAuthGameMode<ACH4GameMode>();
	if (GM)
	{
		GM->OnPlayerDowned(this);
	}
}

// void ACH4PlayerState::OnRep_IsDowned()
// {
// }

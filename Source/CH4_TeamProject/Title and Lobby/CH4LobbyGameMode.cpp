// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4LobbyGameMode.h"
#include "CH4_TeamProject/Game/CH4GameState.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"
#include "Kismet/GameplayStatics.h"

ACH4LobbyGameMode::ACH4LobbyGameMode()
{
}

void ACH4LobbyGameMode::SetPlayerLifeState(ACH4PlayerController* PC)
{
	ACH4PlayerState* PS = PC->GetPlayerState<ACH4PlayerState>();
	PS->SetPlayerLifeState(EPlayerLifeState::Ready);
}

void ACH4LobbyGameMode::SetReadyPlayerCount()
{
	ReadyPlayerCount++;
	if (ReadyPlayerCount >= MaxPlayerCount)
	{
		GetWorld()->ServerTravel(TEXT("/Game/Maps/REALSTAGE"));
	}
}

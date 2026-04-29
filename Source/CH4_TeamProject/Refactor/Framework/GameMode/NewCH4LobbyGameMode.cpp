#include "NewCH4LobbyGameMode.h"

#include "CH4_TeamProject/Game/CH4GameState.h"
#include "CH4_TeamProject/Game/CH4PlayerState.h"
#include "CH4_TeamProject/Refactor/Player/Controller/CH4LobbyPlayerController.h"
#include "Engine/World.h"

ANewCH4LobbyGameMode::ANewCH4LobbyGameMode()
{
	GameStateClass = ACH4GameState::StaticClass();
	PlayerStateClass = ACH4PlayerState::StaticClass();
	PlayerControllerClass = ACH4LobbyPlayerController::StaticClass();

	bUseSeamlessTravel = false;
}

void ANewCH4LobbyGameMode::SetPlayerLifeState(ACH4LobbyPlayerController* PC)
{
	if (!PC) return;

	APlayerState* BasePS = PC->PlayerState;
	if (!BasePS)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerState is NULL"));
		return;
	}

	ACH4PlayerState* PS = Cast<ACH4PlayerState>(BasePS);
	if (!PS)
	{
		UE_LOG(LogTemp, Error, TEXT("PS is not ACH4PlayerState. Class: %s"), *BasePS->GetClass()->GetName());
		return;
	}

	PS->SetPlayerLifeState(EPlayerLifeState::Ready);
	SetReadyPlayerCount();
}

void ANewCH4LobbyGameMode::SetReadyPlayerCount()
{
	ReadyPlayerCount++;

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is NULL"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Ready: %d / %d"), ReadyPlayerCount, MaxPlayerCount);

	if (ReadyPlayerCount >= MaxPlayerCount)
	{
		World->ServerTravel(TEXT("/Game/Maps/L_Stage1"));
	}
}

#include "CH4DefenseGameMode.h"

#include "CH4_TeamProject/Refactor/Framework/GameState/CH4DefenseGameState.h"
#include "CH4_TeamProject/Refactor/Framework/PlayerState/CH4DefensePlayerState.h"
#include "CH4_TeamProject/Refactor/Player/Character/CH4PlayerCharacter.h"
#include "CH4_TeamProject/Refactor/Player/Controller/CH4DefensePlayerController.h"
#include "CH4_TeamProject/Refactor/SpawnPoint/CH4ItemSpawnPoint.h"
#include "CH4_TeamProject/Refactor/SpawnPoint/CH4ZombieSpawnPoint.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ACH4DefenseGameMode::ACH4DefenseGameMode()
{
	GameStateClass = ACH4DefenseGameState::StaticClass();
	PlayerStateClass = ACH4DefensePlayerState::StaticClass();
	PlayerControllerClass = ACH4DefensePlayerController::StaticClass();

	bUseSeamlessTravel = false;

	static ConstructorHelpers::FClassFinder<ACH4PlayerCharacter> PlayerCharacter(TEXT("/Game/Player/PlayerBluePrint/BP_Player.BP_Player_C"));

	if (PlayerCharacter.Succeeded())
	{
		DefaultPawnClass = PlayerCharacter.Class;
	}
}

void ACH4DefenseGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> ItemFoundVolumes;
	TArray<AActor*> ZombieFoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACH4ItemSpawnPoint::StaticClass(), ItemFoundVolumes);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACH4ZombieSpawnPoint::StaticClass(), ZombieFoundVolumes);

	for (AActor* Actor : ItemFoundVolumes)
	{
		if (ACH4ItemSpawnPoint* ItemSpawnVolume = Cast<ACH4ItemSpawnPoint>(Actor))
		{
			ItemSpawnVolume->SpawnItems();
		}
	}

	for (AActor* Actor : ZombieFoundVolumes)
	{
		if (ACH4ZombieSpawnPoint* ZombieSpawnVolume = Cast<ACH4ZombieSpawnPoint>(Actor))
		{
			ZombieSpawnVolume->SpawnZombies(StageStartWaveData);
		}
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(It->Get()))
		{
			PC->Client_EnablePlayerInput();
		}
	}
}

void ACH4DefenseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACH4DefenseGameState* GS = Cast<ACH4DefenseGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	GS->AddAlivePlayerCount();

	if (!bGameStarted && GS->AlivePlayerCount >= 4)
	{
		bGameStarted = true;

		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			PlayGame();
		});
	}
}

void ACH4DefenseGameMode::PlayGame()
{
	if (ACH4DefenseGameState* GS = Cast<ACH4DefenseGameState>(GetWorld()->GetGameState()))
	{
		GS->SetGamePhase(EGamePhase::StartStage);
		GS->SetDayPhase(EDayPhase::Day);
	}

	GetWorld()->ServerTravel(TEXT("/Game/Maps/REALSTAGE"));
}

void ACH4DefenseGameMode::EndGame(EGamePhase GP)
{
	bGameStarted = false;

	if (ACH4DefenseGameState* GS = Cast<ACH4DefenseGameState>(GetWorld()->GetGameState()))
	{
		GS->SetGamePhase(GP);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(It->Get()))
		{
			PC->Client_DisablePlayerInput();
		}
	}

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ACH4DefenseGameMode::OnPlayerDowned(ACH4DefensePlayerState* PlayerState)
{
	if (!PlayerState) return;

	if (PlayerState->PlayerReviveCount <= 0)
	{
		PlayerState->SetLifeState(EPlayerLifeState::Dead);
		return;
	}

	PlayerState->SetLifeState(EPlayerLifeState::Downed);

	ACH4DefenseGameState* GS = Cast<ACH4DefenseGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	GS->SubtractAlivePlayerCount();

	if (GS->AlivePlayerCount > 0)
	{
		if (ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(PlayerState->GetOwner()))
		{
			PC->Client_DisablePlayerInput();
			PC->Client_PlayDownAnim();
			PC->Client_SetPlayerDownedUI(true);
		}
	}
	else
	{
		SetGameResult();
	}
}

void ACH4DefenseGameMode::OnPlayerRevived(ACH4DefensePlayerState* PlayerState)
{
	if (!PlayerState) return;
	if (PlayerState->PlayerReviveCount <= 0) return;

	PlayerState->PlayerReviveCount--;
	PlayerState->SetLifeState(EPlayerLifeState::Alive);

	if (ACH4DefenseGameState* GS = Cast<ACH4DefenseGameState>(GetWorld()->GetGameState()))
	{
		GS->AddAlivePlayerCount();
	}

	if (ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(PlayerState->GetPlayerController()))
	{
		PC->Client_PlayReviveAnim();
		PC->Client_SetPlayerDownedUI(false);
	}
}

void ACH4DefenseGameMode::SetGameResult()
{
	ACH4DefenseGameState* GS = Cast<ACH4DefenseGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	GS->SetDayPhase(EDayPhase::None);

	if (GS->AlivePlayerCount <= 0)
	{
		EndGame(EGamePhase::Lose);
	}
	else if (GS->AlivePlayerCount > 0 && GS->GearPartsCount >= 3)
	{
		EndGame(EGamePhase::Clear);
	}
}

void ACH4DefenseGameMode::RequestReturnToLobby()
{
	if (!HasAuthority()) return;
	if (bIsReturningToLobby) return;

	bIsReturningToLobby = true;
	GetWorld()->ServerTravel(TEXT("/Game/Maps/TitleAndLobby/L_Lobby?listen"));
}

void ACH4DefenseGameMode::SetDayPhaseAtServer(EDayPhase NewPhase)
{
	ACH4DefenseGameState* GS = Cast<ACH4DefenseGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	if (GS->ServerTime >= GS->TotalDayPhaseCycleTime)
	{
		GS->ServerTime = 0;
	}

	if (GS->ServerTime < GS->DayTime)
	{
		GS->SetDayPhase(EDayPhase::Day);
		if (GS->DayPhase == NewPhase) return;

		TArray<AActor*> ZombieFoundVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACH4ZombieSpawnPoint::StaticClass(), ZombieFoundVolumes);

		for (AActor* Actor : ZombieFoundVolumes)
		{
			if (ACH4ZombieSpawnPoint* ZombieSpawnVolume = Cast<ACH4ZombieSpawnPoint>(Actor))
			{
				ZombieSpawnVolume->SpawnZombies(DayWaveData);
			}
		}
	}
	else if (GS->ServerTime < GS->DayTime + GS->EveningTime)
	{
		GS->SetDayPhase(EDayPhase::Evening);
		if (GS->DayPhase == NewPhase) return;
	}
	else
	{
		GS->SetDayPhase(EDayPhase::Night);
		if (GS->DayPhase == NewPhase) return;

		TArray<AActor*> ZombieFoundVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACH4ZombieSpawnPoint::StaticClass(), ZombieFoundVolumes);

		for (AActor* Actor : ZombieFoundVolumes)
		{
			if (ACH4ZombieSpawnPoint* ZombieSpawnVolume = Cast<ACH4ZombieSpawnPoint>(Actor))
			{
				ZombieSpawnVolume->SpawnZombies(NightWaveData);
			}
		}
	}

	if (GS->AlivePlayerCount >= 0 && GS->GearPartsCount >= 3)
	{
		SetGameResult();
		GS->SetGamePhase(EGamePhase::FinalDefense);
	}
}

void ACH4DefenseGameMode::FinalDefenseWaveSpawn()
{
	TArray<AActor*> ZombieFoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACH4ZombieSpawnPoint::StaticClass(), ZombieFoundVolumes);

	for (AActor* Actor : ZombieFoundVolumes)
	{
		if (ACH4ZombieSpawnPoint* ZombieSpawnVolume = Cast<ACH4ZombieSpawnPoint>(Actor))
		{
			ZombieSpawnVolume->SpawnZombies(FinalWaveData);
		}
	}
}

void ACH4DefenseGameMode::StartDefenseTimer()
{
	const ACH4DefenseGameState* GS = GetWorld() ? Cast<ACH4DefenseGameState>(GetWorld()->GetGameState()) : nullptr;
	if (!GS) return;

	if (DefenseWaveTimer >= 6.0f * 60.0f || GS->AlivePlayerCount <= 0)
	{
		SetGameResult();
	}
}

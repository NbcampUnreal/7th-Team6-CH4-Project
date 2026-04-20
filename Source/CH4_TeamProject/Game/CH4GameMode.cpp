
#include "CH4GameMode.h"

#include "CH4_TeamProject/Player/CH4Character.h"
#include "UObject/ConstructorHelpers.h"
#include "CH4_TeamProject/Game/CH4GameState.h"
#include "CH4_TeamProject/Game/CH4PlayerState.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "CH4_TeamProject/Item/Consumable/ItemSpawnPoint.h"

ACH4GameMode::ACH4GameMode()
{	
	GameStateClass = ACH4GameState::StaticClass();
	PlayerStateClass = ACH4PlayerState::StaticClass();
	PlayerControllerClass = ACH4PlayerController::StaticClass();
	
	bUseSeamlessTravel = false;
	bIsReturningToLobby = false;
	
	static ConstructorHelpers::FClassFinder<ACH4Character>
		PlayerCharacter(TEXT("/Game/Player/PlayerBluePrint/BP_Player.BP_Player_C"));

	if (PlayerCharacter.Class)
	{
		DefaultPawnClass = PlayerCharacter.Class;
	}
}

void ACH4GameMode::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemSpawnPoint::StaticClass(), FoundVolumes);
	
	for (AActor* Actor : FoundVolumes)
	{
		AItemSpawnPoint* SpawnVolume = Cast<AItemSpawnPoint>(Actor);
		if (SpawnVolume)
		{
			SpawnVolume->SpawnItems();
		}
	}
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(It->Get());
		PC->Client_EnablePlayerInput();
	}
	
	
}

void ACH4GameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->AddAlivePlayerCount();
	}

	UE_LOG(LogTemp, Warning, TEXT("Player Joined: %d"), GS->AlivePlayerCount);

	if (!bGameStarted && GS->AlivePlayerCount >= 4)
	{
		bGameStarted = true;
		
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			PlayGame();
		});
	}
}

void ACH4GameMode::PlayGame()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayGame CALLED"));
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->SetGamePhase(EGamePhase::StartStage);
		GS->SetDayPhase(EDayPhase::Day);
	}
	
	GetWorld()->ServerTravel(TEXT("/Game/Maps/REALSTAGE"));
}

void ACH4GameMode::EndGame(EGamePhase GP)
{	
	bGameStarted = false;
	// GameState에 반영
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->SetGamePhase(GP);
		GS->SetDayPhase(EDayPhase::None);
	}
	
	// 현재 월드에 존재하는 모든 PlayerController를 순회(반복)하기 위한 반복자(iterator)
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(It->Get());
		if (PC)
		{
			PC->Client_DisablePlayerInput();
		}
	}
	
	GetWorldTimerManager().ClearAllTimersForObject(this); 
}

void ACH4GameMode::OnPlayerDowned(ACH4PlayerState* PlayerState)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Downed"));
	
	if (PlayerState->PlayerReviveCount <= 0) // 데카 0
	{
		PlayerState->SetLifeState(EPlayerLifeState::Dead);
		return;
	}
	
	PlayerState->SetLifeState(EPlayerLifeState::Downed); // 플레이어 상태 변경
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->SubtractAlivePlayerCount(); // 생존자 수 감소
	}
	
	if (GS->AlivePlayerCount > 0)
	{
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(PlayerState->GetOwner()); // 해당 컨트롤러를 소유한 객체를 ()에서 불러오기
		if (PC)
		{
			PC->Client_DisablePlayerInput();
			PC->Client_PlayDownAnim();
			PC->Client_SetPlayerDownedUI(true);
		}
	}
	else // 모든 플레이어 다운 시
	{
		SetGameResult();
	}
}

void ACH4GameMode::OnPlayerRevived(ACH4PlayerState* PlayerState)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Revived"));
	
	if (PlayerState->PlayerReviveCount <= 0)
		return;

	PlayerState->PlayerReviveCount--;
	PlayerState->SetLifeState(EPlayerLifeState::Alive);
	
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->AddAlivePlayerCount();
	}
	
	ACH4PlayerController* PC = Cast<ACH4PlayerController>(PlayerState->GetPlayerController());
	if (PC)
	{
		/*PC->Client_EnablePlayerInput();*/
		PC->Client_PlayReviveAnim();
		PC->Client_SetPlayerDownedUI(false);
	}
}

void ACH4GameMode::SetGameResult()
{
	ACH4GameState* GS = Cast<ACH4GameState>(GetWorld()->GetGameState());
	GS->SetDayPhase(EDayPhase::None);
	
	if (GS && GS->AlivePlayerCount <= 0)
	{ 
		EndGame(EGamePhase::Lose);
	}
	else if (GS && GS->AlivePlayerCount > 0 && GS->GearPartsCount == 3)
	{
		EndGame(EGamePhase::Clear);
	}
}


void ACH4GameMode::RequestReturnToLobby()
{
	if (!HasAuthority())
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("=== 게임 종료: 모든 플레이어를 로비로 보냅니다. ==="));
	
	if (bIsReturningToLobby)
	{
		return;
	}
	
	bIsReturningToLobby = true;
	FString LobbyMapPath = TEXT("/Game/Maps/TitleAndLobby/L_Lobby?listen");
	GetWorld()->ServerTravel(LobbyMapPath);
}

// void ACH4GameMode::UpdateMainServerTime() const
// {
// 	if (ACH4GameState* GS = GetGameState<ACH4GameState>())
// 	{
// 		GS->SetServerTime(GetWorld()->GetTimeSeconds());
// 	}
// }



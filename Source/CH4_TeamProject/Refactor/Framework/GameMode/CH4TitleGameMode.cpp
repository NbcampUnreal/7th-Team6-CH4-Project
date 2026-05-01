#include "CH4TitleGameMode.h"

#include "CH4_TeamProject/Refactor/Player/Controller/CH4TitlePlayerController.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

ACH4TitleGameMode::ACH4TitleGameMode()
{
	PlayerControllerClass = ACH4TitlePlayerController::StaticClass();
	bUseSeamlessTravel = false;
}

void ACH4TitleGameMode::EnjoyToServerLobby()
{
	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC) return;

	PC->ClientTravel(TEXT("43.200.5.254:7777"), TRAVEL_Absolute);
}

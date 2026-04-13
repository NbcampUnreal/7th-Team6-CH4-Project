
#include "CH4Title.h"

#include "Blueprint/UserWidget.h"

ACH4Title::ACH4Title()
{
}

void ACH4Title::EnjoyToServerLobby()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	PC->ClientTravel(TEXT("127.0.0.1:7777"), TRAVEL_Absolute);
}

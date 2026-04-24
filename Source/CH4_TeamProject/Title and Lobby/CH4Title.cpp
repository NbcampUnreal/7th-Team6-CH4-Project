
#include "CH4Title.h"

#include "Blueprint/UserWidget.h"

ACH4Title::ACH4Title()
{
}

void ACH4Title::EnjoyToServerLobby()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	PC->ClientTravel(TEXT("43.200.5.254:7777"), TRAVEL_Absolute);
}

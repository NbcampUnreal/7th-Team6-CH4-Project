#include "CH4LobbyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "CH4_TeamProject/Refactor/Framework/GameMode/NewCH4LobbyGameMode.h"
#include "Engine/World.h"

void ACH4LobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	if (!HUDServerLobbyWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDServerLobbyWidgetClass is null"));
		return;
	}

	HUDServerLobbyWidgetInstance = CreateWidget<UUserWidget>(this, HUDServerLobbyWidgetClass);
	if (HUDServerLobbyWidgetInstance)
	{
		HUDServerLobbyWidgetInstance->AddToViewport();
		bShowMouseCursor = true;
	}
}

void ACH4LobbyPlayerController::Server_RequestReady_Implementation()
{
	if (ANewCH4LobbyGameMode* GM = GetWorld()->GetAuthGameMode<ANewCH4LobbyGameMode>())
	{
		GM->SetPlayerLifeState(this);
	}
}

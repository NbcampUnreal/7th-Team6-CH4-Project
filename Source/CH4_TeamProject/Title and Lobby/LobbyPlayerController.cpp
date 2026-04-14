// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "CH4LobbyGameMode.h"
ALobbyPlayerController::ALobbyPlayerController()
{

	
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController()) return;

	if (!HUDServerLobbyWidgetClass) 
	{
		UE_LOG(LogTemp, Error, TEXT("HUDServerLobbyWidgetClass is nullptr!"));
		return;
	}
	
	HUDServerLobbyWidgetInstance = CreateWidget<UUserWidget>(this,HUDServerLobbyWidgetClass);
	if (HUDServerLobbyWidgetInstance)
	HUDServerLobbyWidgetInstance->AddToViewport();
}

void ALobbyPlayerController::HideCurrentWidget()
{
	Super::HideCurrentWidget();
}

void ALobbyPlayerController::ShowGameRule()
{
	Super::ShowGameRule();
}

void ALobbyPlayerController::Server_RequestReady_Implementation()
{
	ACH4LobbyGameMode* GM = GetWorld()->GetAuthGameMode<ACH4LobbyGameMode>();
	if (GM)
	{
		GM->SetPlayerLifeState(this);
		UE_LOG(LogTemp,Error,TEXT("컨트롤러에서 호출은됨"))
	}
}

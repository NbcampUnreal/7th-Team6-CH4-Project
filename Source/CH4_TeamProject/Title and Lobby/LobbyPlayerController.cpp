// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

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

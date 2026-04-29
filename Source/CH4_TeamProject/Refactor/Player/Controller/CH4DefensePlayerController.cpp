#include "CH4DefensePlayerController.h"

#include "CH4_TeamProject/Refactor/Framework/GameMode/CH4DefenseGameMode.h"
#include "CH4_TeamProject/Refactor/Player/Character/CH4PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ACH4DefensePlayerController::ACH4DefensePlayerController()
{
	bReplicates = true;
	bShowMouseCursor = false;
}

FGenericTeamId ACH4DefensePlayerController::GetGenericTeamId() const
{
	return ControlledCharacter ? ControlledCharacter->GetGenericTeamId() : FGenericTeamId();
}

void ACH4DefensePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	StartGame();
}

void ACH4DefensePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCharacter = Cast<ACH4PlayerCharacter>(InPawn);
}

void ACH4DefensePlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	ControlledCharacter = nullptr;
}

void ACH4DefensePlayerController::StartGame()
{
	if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
	{
		ViewportClient->RemoveAllViewportWidgets();
	}

	if (CurrentMenuWidget)
	{
		CurrentMenuWidget->RemoveFromParent();
		CurrentMenuWidget = nullptr;
	}

	if (!HUDWidgetClass) return;

	CurrentMenuWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
	if (!CurrentMenuWidget) return;

	CurrentMenuWidget->AddToViewport();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void ACH4DefensePlayerController::ShowGameClear()
{
	HideCurrentWidget();

	if (!GameClearWidgetClass) return;

	CurrentGameClearWidget = CreateWidget<UUserWidget>(this, GameClearWidgetClass);
	if (!CurrentGameClearWidget) return;

	CurrentGameClearWidget->AddToViewport();
	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(CurrentGameClearWidget->TakeWidget());
	SetInputMode(InputMode);

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ACH4DefensePlayerController::ShowGameOver()
{
	if (!GameOverWidgetClass) return;

	CurrentGameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
	if (!CurrentGameOverWidget) return;

	CurrentGameOverWidget->AddToViewport();
	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(CurrentGameOverWidget->TakeWidget());
	SetInputMode(InputMode);
}

void ACH4DefensePlayerController::HideCurrentWidget()
{
	if (CurrentMenuWidget)
	{
		CurrentMenuWidget->RemoveFromParent();
		CurrentMenuWidget = nullptr;
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ACH4DefensePlayerController::BackToLobby_Implementation()
{
	if (!HasAuthority()) return;

	if (ACH4DefenseGameMode* GM = GetWorld()->GetAuthGameMode<ACH4DefenseGameMode>())
	{
		GM->RequestReturnToLobby();
	}
}

void ACH4DefensePlayerController::Client_DisablePlayerInput_Implementation()
{
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
}

void ACH4DefensePlayerController::Client_EnablePlayerInput_Implementation()
{
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
}

void ACH4DefensePlayerController::Client_PlayDownAnim_Implementation()
{
	if (ACH4PlayerCharacter* MyChar = Cast<ACH4PlayerCharacter>(GetPawn()))
	{
		MyChar->PlayDownAnimation();
	}
}

void ACH4DefensePlayerController::Client_PlayReviveAnim_Implementation()
{
	if (ACH4PlayerCharacter* MyChar = Cast<ACH4PlayerCharacter>(GetPawn()))
	{
		MyChar->PlayReviveAnimation();
	}
}

void ACH4DefensePlayerController::Client_MoveToLobby_Implementation()
{
}

void ACH4DefensePlayerController::Client_InvokeGameClearUI_Implementation()
{
	HideCurrentWidget();

	if (!HUDGameClearWidgetClass) return;

	CurrentWidget = CreateWidget<UUserWidget>(this, HUDGameClearWidgetClass);
	if (CurrentWidget)
	{
		CurrentWidget->AddToViewport();
	}
}

void ACH4DefensePlayerController::Client_InvokeGameLoseUI_Implementation()
{
	if (!HUDGameLoseWidgetClass) return;

	HUDGameLoseWidgetInstance = CreateWidget<UUserWidget>(this, HUDGameLoseWidgetClass);
	if (HUDGameLoseWidgetInstance)
	{
		HUDGameLoseWidgetInstance->AddToViewport();
	}
}

void ACH4DefensePlayerController::Client_SetPlayerDownedUI_Implementation(bool bShow)
{
	if (!IsLocalController()) return;

	if (bShow)
	{
		if (HUDPlayerDownedWidgetInstance) return;
		if (!HUDPlayerDownedWidgetClass) return;

		HUDPlayerDownedWidgetInstance = CreateWidget<UUserWidget>(this, HUDPlayerDownedWidgetClass);
		if (HUDPlayerDownedWidgetInstance)
		{
			HUDPlayerDownedWidgetInstance->AddToViewport();
		}
	}
	else
	{
		if (HUDPlayerDownedWidgetInstance)
		{
			HUDPlayerDownedWidgetInstance->RemoveFromParent();
			HUDPlayerDownedWidgetInstance = nullptr;
		}
	}
}

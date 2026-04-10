
#include "CH4PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "CH4_TeamProject/Player/CH4Character.h"
#include "CH4_TeamProject/DataBase/DataBase.h"

ACH4PlayerController::ACH4PlayerController()
{
    bReplicates = true;
    bShowMouseCursor = true;
    
    
}

void ACH4PlayerController::BeginPlay()
{
    if (!IsLocalController())
        return;
    
    Client_MoveToLobby();
    
    // HUD 위젯 생성 및 표시
    if (HUDWidgetClass)
    {
        UUserWidget* HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }

    CurrentMenuWidget = nullptr;

    // TODO: 시작 시 어떤 메뉴를 먼저 띄울지 함수 호출 위치 결정
    ShowStartMenu();
}

void ACH4PlayerController::BeginPlayingState()
{
    Super::BeginPlayingState();

}

void ACH4PlayerController::ShowStartMenu()
{
    HideCurrentMenu(); // 기존에 떠 있는 게 있다면 지웁니다.

    if (!StartMenuClass) return;

    // 1. 위젯 생성 (설계도인 Class로 실체인 Instance를 만듭니다)
    CurrentMenuWidget = CreateWidget<UUserWidget>(this, StartMenuClass);

    if (CurrentMenuWidget)
    {
        // 2. 화면에 붙이기
        CurrentMenuWidget->AddToViewport();

        // 3. 마우스 설정: 시작 메뉴에서는 버튼을 눌러야 하니 마우스를 보여줍니다.
        bShowMouseCursor = true;

        // 4. 입력 모드: 게임 캐릭터 조작은 막고 UI만 만지게 설정합니다.
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(CurrentMenuWidget->TakeWidget());
        SetInputMode(InputMode);
    }
}

void ACH4PlayerController::ShowGameOver()
{
}

void ACH4PlayerController::HideCurrentMenu()
{
}

void ACH4PlayerController::Client_DisablePlayerInput_Implementation()
{    
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    // SetIgnoreJumpInput(true); -> 점프 구현 시 주석 풀기
}

void ACH4PlayerController::Client_EnablePlayerInput_Implementation()
{    
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
    // SetIgnoreJumpInput(false); -> 점프 구현 시 주석 풀기
}

void ACH4PlayerController::Client_PlayDownAnim_Implementation()
{
    ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());
    if (MyChar)
    {
        // MyChar->PlayDownAnimation();
    }
}

void ACH4PlayerController::Client_PlayReviveAnim_Implementation()
{
    ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());
    if (MyChar)
    {
        // MyChar->PlayReviveAnimation();
    }
}

void ACH4PlayerController::Client_MoveToLobby_Implementation()
{
    HUDLobbyWidgetInstance = CreateWidget<UUserWidget>(this,HUDLobbyWidgetClass);
    HUDLobbyWidgetInstance->AddToViewport();
}

void ACH4PlayerController::Client_InvokeGameClearUI_Implementation()
{
    HUDGameClearWidgetInstance = CreateWidget<UUserWidget>(this,HUDGameClearWidgetClass);
    HUDGameClearWidgetInstance->AddToViewport();
}

void ACH4PlayerController::Client_InvokeGameLoseUI_Implementation()
{
    HUDGameLoseWidgetInstance = CreateWidget<UUserWidget>(this,HUDGameLoseWidgetClass);
    HUDGameLoseWidgetInstance->AddToViewport();
}

void ACH4PlayerController::Client_SetPlayerDownedUI_Implementation(bool bShow)
{
    if (!IsLocalController())
        return;
    
    if (bShow)
    {
        if (!HUDPlayerDownedWidgetInstance)
        {
            HUDPlayerDownedWidgetInstance = CreateWidget<UUserWidget>(this,HUDPlayerDownedWidgetClass);
            if (HUDPlayerDownedWidgetInstance)
            {
                HUDPlayerDownedWidgetInstance->AddToViewport();
            }
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

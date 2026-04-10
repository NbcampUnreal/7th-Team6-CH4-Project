
#include "CH4PlayerController.h"
#include "Blueprint/UserWidget.h"

ACH4PlayerController::ACH4PlayerController()
{
    bReplicates = true;
}

void ACH4PlayerController::BeginPlay()
{
    // HUD 위젯 생성 및 표시
    if (HUDWidgetClass)
    {
        UUserWidget* HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }
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
    // ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());  -> 다운 애니메이션 찾으면 주석 풀기
    // if (MyChar)
    // {
    //     MyChar->PlayDownAnimation();
    // }
}

void ACH4PlayerController::Client_PlayReviveAnim_Implementation()
{
    // ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());  -> 소생 애니메이션 찾으면 주석 풀기
    // if (MyChar)
    // {
    //     MyChar->PlayReviveAnimation();
    // }
}

void ACH4PlayerController::Client_InvokeDownUI_Implementation()
{
    // 회색 화면 등 -> 구현 필요
}

void ACH4PlayerController::Client_HideDownUI_Implementation()
{
    // 다운 UI 제거
}

void ACH4PlayerController::Client_MoveToLobby_Implementation() const
{
    // 로비 UI로 이동
}

void ACH4PlayerController::Client_InvokeGameClearUI_Implementation() const
{
    // 클리어 UI 띄우기
}

void ACH4PlayerController::Client_InvokeGameLoseUI_Implementation() const
{
    // 패배 UI 띄우기
}
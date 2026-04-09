#include "CH4PlayerController.h"
#include "Blueprint/UserWidget.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4PlayerController.h"

ACH4PlayerController::ACH4PlayerController()
{
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

    CurrentMenuWidget = nullptr;

    // TODO: 시작 시 어떤 메뉴를 먼저 띄울지 함수 호출 위치 결정
    ShowStartMenu();


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

void ACH4PlayerController::Client_HandlePlayerDowned_Implementation()
{
    // 다운 애니메이션 재생 명령(Client RPC), 컨트롤 무력화, 회색 UI 명령
    
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    // SetIgnoreJumpInput(true); -> 점프 구현 시 주석 풀기
    
    // ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());  -> 다운 애니메이션 찾으면 주석 풀기
    // if (MyChar)
    // {
    //     MyChar->PlayDownAnimation();
    // }
    
    // ShowDownedUI(); 회색 화면 등 -> 구현 필요
}

void ACH4PlayerController::Client_HandlePlayerRevived_Implementation()
{
    // 애니메이션 원래대로, 조작 가능하도록 변경, UI 원상복구
    
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
    // SetIgnoreJumpInput(false); -> 점프 구현 시 주석 풀기
    
    // ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());  -> 소생 애니메이션 찾으면 주석 풀기
    // if (MyChar)
    // {
    //     MyChar->PlayReviveAnimation();
    // }
    
    // UI 복구
    // HideDownedUI(); -> UI 구현 시 주석 풀기
}

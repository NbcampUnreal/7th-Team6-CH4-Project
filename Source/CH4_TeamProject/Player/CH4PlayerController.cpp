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

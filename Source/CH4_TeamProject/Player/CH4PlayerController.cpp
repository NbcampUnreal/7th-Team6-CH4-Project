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

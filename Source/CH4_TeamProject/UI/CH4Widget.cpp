// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4Widget.h"
#include "TimerManager.h" // 필수!
#include "Engine/World.h"

void UCH4Widget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("Widget Construct됨"));

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            WidgetTimerHandle,
            this,
            &UCH4Widget::UpdateTimer,
            1.0f,
            true
        );
    }
}

void UCH4Widget::UpdateTimer()
{
    if (RemainingTime > 0)
    {
        RemainingTime -= 1.0f;

        UE_LOG(LogTemp, Warning, TEXT("남은 시간: %f"), RemainingTime);

        if (TimerText)
        {
            TimerText->SetText(FText::AsNumber((int32)RemainingTime));
        }

    }
    else
    {
        // 시간이 0이 되면 타이머 중지
        GetWorld()->GetTimerManager().ClearTimer(WidgetTimerHandle);
        if (TimerText)
        {
            TimerText->SetText(FText::FromString(TEXT("끝")));
        }
    }
}


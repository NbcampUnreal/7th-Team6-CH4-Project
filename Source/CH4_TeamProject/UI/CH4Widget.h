// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "CH4Widget.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API UCH4Widget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
    float RemainingTime = 60.0f;

    FTimerHandle WidgetTimerHandle;

    void UpdateTimer();

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerText;
};

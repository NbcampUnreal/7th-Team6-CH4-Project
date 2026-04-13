// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h" 
#include "CH4HUD.generated.h"


/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API ACH4HUD : public AHUD
{
	GENERATED_BODY()
	


protected:

        // 화면 중앙에 그려집니다.
        UPROPERTY(EditDefaultsOnly)
        UTexture2D* CrosshairTexture;

public:

              // HUD의 기본 드로 콜입니다.
    virtual void DrawHUD() override;
};
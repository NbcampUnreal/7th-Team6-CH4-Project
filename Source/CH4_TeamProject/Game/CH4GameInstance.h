// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CH4GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API UCH4GameInstance : public UGameInstance
{
	GENERATED_BODY()
    

public:

    UCH4GameInstance();

    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddToScore(int32 Amount);

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
    int32 TotalScore;

  
    // 현재 레벨 인덱스 (GameState에서도 관리할 수 있지만, 맵 전환 후에도 살리고 싶다면 GameInstance에 복제할 수 있음)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
    int32 CurrentLevelIndex;

    // 블루프린트에서 접근 가능하도록 BlueprintReadOnly 추가
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
    float RemainingTime = 60.0f;

    // 타이머 업데이트 함수 (Public)
    void UpdateTimer();
};

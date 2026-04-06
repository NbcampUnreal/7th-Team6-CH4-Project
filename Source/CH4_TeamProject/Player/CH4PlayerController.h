// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CH4PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API ACH4PlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	ACH4PlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

protected:
	virtual void BeginPlay() override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CH4PlayerController.generated.h"

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
	
public:
	//----합칠 수 있을듯----(bool 매개변수를 통해서)
	UFUNCTION(Client, Reliable)
	void Client_DisablePlayerInput();
	
	UFUNCTION(Client, Reliable)
	void Client_EnablePlayerInput();
	//---------------------------
	
	
	//----합칠 수 있을듯----(EPlayerLifeState 매개변수에 따라서)
	UFUNCTION(Client, Reliable)
	void Client_PlayDownAnim();
	
	UFUNCTION(Client, Reliable)
	void Client_PlayReviveAnim();
	//---------------------------
	
	
	//----합칠 수 있을듯----
	UFUNCTION(Client, Reliable)
	void Client_InvokeDownUI() ;
	
	UFUNCTION(Client, Reliable)
	void Client_HideDownUI();
	
	UFUNCTION(Client, Reliable)
	void Client_MoveToLobby() const;
	
	UFUNCTION(Client, Reliable)
	void Client_InvokeGameClearUI() const;
	
	UFUNCTION(Client, Reliable)
	void Client_InvokeGameLoseUI() const;
	//---------------------------
};

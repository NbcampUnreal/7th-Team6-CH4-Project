// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/WidgetComponent.h"
#include "CH4PlayerController.generated.h"

UCLASS(Abstract)
class CH4_TEAMPROJECT_API ACH4PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACH4PlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

protected:
	virtual void BeginPlay() override;
	
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintCallable)
	void ShowStartMenu();


	UFUNCTION(BlueprintCallable)
	void ShowGameOver();

	UFUNCTION(BlueprintCallable)
	void HideCurrentMenu();
	
public:
	UPROPERTY(EditAnywhere, Category = "MyUI")
	TSubclassOf<UUserWidget> StartMenuClass; // 시작 화면용 주머니

	UPROPERTY()
	UUserWidget* CurrentMenuWidget;
	
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
	void Client_MoveToLobby();
	
	UFUNCTION(Client, Reliable)
	void Client_InvokeGameClearUI();
	
	UFUNCTION(Client, Reliable)
	void Client_InvokeGameLoseUI() const;
	//---------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My UI")
	TSubclassOf<UUserWidget> HUDLobbyWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "My UI")
	UUserWidget* HUDLobbyWidgetInstance;
	
	
};

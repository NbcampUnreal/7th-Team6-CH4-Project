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


	//
	UPROPERTY(EditAnywhere, Category = "MyUI")
	TSubclassOf<UUserWidget> StartMenuClass; // 시작 화면용 주머니

	// 룰 화면 위젯 블루프린트 클래스를 에디터에서 할당받습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameRulesWidgetClass;

	// 현재 열려 있는 룰 위젯의 인스턴스를 저장합니다. (nullptr 필수!)
	UPROPERTY()
	UUserWidget* CurrentRulesWidget = nullptr;


	//아이템 완성시 주석해제
	//UPROPERTY(BlueprintReadOnly, Category = "GameData")
	//int32 GamePhase;

	//UPROPERTY(BlueprintReadOnly, Category = "GameData")
	//int32 GearPartsCount;


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ShowStartMenu();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowGameRule();

	UFUNCTION(BlueprintCallable)
	void ShowGameOver();

	UFUNCTION(BlueprintCallable)
	void HideCurrentMenu();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ExitGame();

	
	virtual void BeginPlayingState() override;
	void ShowStartMenu();
	void ShowGameOver();
	void HideCurrentMenu();

public:
	UPROPERTY(EditAnywhere, Category = "MyUI")
	TSubclassOf<UUserWidget> StartMenuClass; // 시작 화면용 주머니

	UPROPERTY()
	UUserWidget* CurrentMenuWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My UI")
	TSubclassOf<UUserWidget> HUDLobbyWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "My UI")
	UUserWidget* HUDLobbyWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My UI")
	TSubclassOf<UUserWidget> HUDGameClearWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "My UI")
	UUserWidget* HUDGameClearWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My UI")
	TSubclassOf<UUserWidget> HUDGameLoseWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "My UI")
	UUserWidget* HUDGameLoseWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My UI")
	TSubclassOf<UUserWidget> HUDPlayerDownedWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "My UI")
	UUserWidget* HUDPlayerDownedWidgetInstance;
	
	UFUNCTION(Client, Reliable)
	void Client_DisablePlayerInput();
	
	UFUNCTION(Client, Reliable)
	void Client_EnablePlayerInput();
	
	UFUNCTION(Client, Reliable)
	void Client_PlayDownAnim();

	UFUNCTION(Client, Reliable)
	void Client_PlayReviveAnim();
	
	UFUNCTION(Client, Reliable)
	void Client_MoveToLobby();
	
	UFUNCTION(Client, Reliable)
	void Client_InvokeGameClearUI();
	
	UFUNCTION(Client, Reliable)
	void Client_InvokeGameLoseUI();	
	
	UFUNCTION(Client, Reliable)
	void Client_SetPlayerDownedUI(bool bShow);
};

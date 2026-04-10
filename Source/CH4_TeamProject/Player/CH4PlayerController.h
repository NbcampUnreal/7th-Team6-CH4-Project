// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/WidgetComponent.h"
#include "CH4PlayerController.generated.h"

UCLASS()
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


	UFUNCTION(BlueprintCallable)
	void ShowGameOver();

	UFUNCTION(BlueprintCallable)
	void HideCurrentMenu();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ExitGame();

	
public:

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowGameRule();

	void Client_EnablePlayerInput_Implementation();



	UPROPERTY()
	UUserWidget* CurrentMenuWidget;
	
	//----합칠 수 있을듯----(bool 매개변수를 통해서)
	UFUNCTION(Client, Reliable)
	void Client_DisablePlayerInput();
	
	//UFUNCTION(Client, Reliable)
	////void Client_HandlePlayerRevived();
	//////
	////


	////void Client_EnablePlayerInput();
	//////---------------------------
	//
	//
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

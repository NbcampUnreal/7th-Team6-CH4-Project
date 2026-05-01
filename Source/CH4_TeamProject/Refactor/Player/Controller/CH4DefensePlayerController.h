/*
 * [Refactor] ACH4DefensePlayerController
 *
 * 출처: Player/CH4PlayerController.h/cpp 의 ACH4PlayerController 중 방어전 모드 전용 멤버
 * 변경:
 *   - 부모 ACH4PlayerController(추상) -> APlayerController 로 직접 상속
 *     (원본 추상 베이스가 방어전 코드를 모두 안고 있어, 본 단계에서는 방어전 도메인을
 *      독립 클래스로 분리. 원본 추상 베이스는 컷오버 시 정리)
 *   - IGenericTeamAgentInterface 직접 구현 (ACH4PlayerCharacter 의 TeamID 위임)
 *   - 폴더 위치 Player/ -> Refactor/Player/Controller/
 *   - 의존 타입을 모두 Refactor/ 의 리팩토링 클래스로 전환:
 *       ACH4Character        -> ACH4PlayerCharacter
 *       ACH4GameMode         -> ACH4DefenseGameMode (BackToLobby)
 *   - 방어전과 무관한 멤버 제외 (Lobby/Title 도메인 책임이라 본 클래스에 두지 않음):
 *       StartMenuClass, GameRulesWidgetClass, ShowStartMenu, ShowGameRule,
 *       HUDLobbyWidgetClass, HUDLobbyWidgetInstance, CurrentRulesWidget
 *   - raw 포인터 -> TObjectPtr 전환, UPROPERTY Category 부여
 *   - BackToLobby Server RPC 진입부에 HasAuthority 가드 유지
 *   - 임시 LogTemp 정리 (영역별 로그 카테고리 도입은 후속 작업)
 *   - 본체 동등 (StartGame, ShowGameClear, ShowGameOver, BackToLobby, Client_*)
 * 사유: 추상 베이스에 섞여있던 방어전 도메인 책임을 독립 클래스로 분리
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "CH4DefensePlayerController.generated.h"

class UUserWidget;
class ACH4PlayerCharacter;

UCLASS()
class CH4_TEAMPROJECT_API ACH4DefensePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACH4DefensePlayerController();

	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void ShowGameClear();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void ShowGameOver();

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void HideCurrentWidget();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "GameFlow")
	void BackToLobby();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameClearWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDGameClearWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDGameLoseWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDPlayerDownedWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentMenuWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentGameClearWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentGameOverWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDGameClearWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDGameLoseWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDPlayerDownedWidgetInstance;

	UPROPERTY()
	TObjectPtr<ACH4PlayerCharacter> ControlledCharacter;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};

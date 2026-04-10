
#include "CH4PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "CH4_TeamProject/Player/CH4Character.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include <Kismet\KismetSystemLibrary.h>
#include "Kismet/GameplayStatics.h"

ACH4PlayerController::ACH4PlayerController()
{
    bReplicates = true;
    bShowMouseCursor = true;
    
    
}

void ACH4PlayerController::BeginPlay()
{
    if (!IsLocalController())
    {
        return;
    }
    
    // TODO: 시작 시 어떤 메뉴를 먼저 띄울지 함수 호출 위치 결정
    ShowStartMenu();
}

void ACH4PlayerController::BeginPlayingState()
{
    Super::BeginPlayingState();

}

void ACH4PlayerController::ShowStartMenu()
{
    HideCurrentWidget(); // 기존에 떠 있는 게 있다면 지웁니다.

    if (!StartMenuClass) return;

    // 1. 위젯 생성 (설계도인 Class로 실체인 Instance를 만듭니다)
    CurrentWidget = CreateWidget<UUserWidget>(this, StartMenuClass);

    if (CurrentWidget)
    {
        // 2. 화면에 붙이기
        CurrentWidget->AddToViewport();

        // 3. 마우스 설정: 시작 메뉴에서는 버튼을 눌러야 하니 마우스를 보여줍니다.
        bShowMouseCursor = true;

        // 4. 입력 모드: 게임 캐릭터 조작은 막고 UI만 만지게 설정합니다.
        FInputModeUIOnly InputMode;
        SetInputMode(InputMode);
    }
}

void ACH4PlayerController::ShowGameOver()
{
}

void ACH4PlayerController::HideCurrentWidget()
{
    // 1. 만약 현재 떠 있는 메뉴(CurrentMenuWidget)가 있다면?
    if (CurrentWidget)
    {
        // 화면에서 지우고
        CurrentWidget->RemoveFromParent();

        // 메모리 주소도 깨끗하게 비웁니다 (방금 배운 nullptr!)
        CurrentWidget = nullptr;
    }

    // 3. 입력을 다시 '게임 전용'으로 바꿉니다. (그래야 캐릭터가 움직입니다)
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
}

void ACH4PlayerController::StartGame()
{
    UE_LOG(LogTemp, Warning, TEXT("Level Start!"));
    HideCurrentWidget();
    
    UGameplayStatics::OpenLevel(this, FName("REALSTAGE"));
    
    // 3. 게임 화면이 나왔으니 다시 HUD(체력바 등)를 띄워줌
    CurrentWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
    if (HUDWidgetClass)
    {
        CurrentWidget->AddToViewport();
        
        // 4. ★핵심: 입력 모드를 게임 전용으로! (이걸 안 하면 화면만 비고 조작이 안 됨)
        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
        bShowMouseCursor = false;
    }
}

void ACH4PlayerController::ExitGame()
{
    // UKismetSystemLibrary::QuitGame(월드 context, 컨트롤러, 종료 사유, 배경 실행 여부)
    UE_LOG(LogTemp, Warning, TEXT("=== C++ ExitGame 진입 성공! ==="));
    UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}

void ACH4PlayerController::ShowGameRule()
{
// 1. 이미 열린 게 있다면 지웁니다.
    HideCurrentWidget();

    // 2. 클래스 체크
    if (!GameRulesWidgetClass) return;

    // 3. 룰 위젯 생성 및 저장
    CurrentWidget = CreateWidget<UUserWidget>(this, GameRulesWidgetClass);
    if (CurrentWidget)
    {
        // 4. 화면에 띄우기 (AddToViewport)
        CurrentWidget->AddToViewport();
        
        FInputModeUIOnly InputMode;
        SetInputMode(InputMode);
        bShowMouseCursor = true;
    }

    CurrentWidget = nullptr;

    // TODO: 시작 시 어떤 메뉴를 먼저 띄울지 함수 호출 위치 결정
    ShowStartMenu();
}

void ACH4PlayerController::Client_DisablePlayerInput_Implementation()
{
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    // SetIgnoreJumpInput(true); -> 점프 구현 시 주석 풀기
}

void ACH4PlayerController::Client_EnablePlayerInput_Implementation()
{
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
    // SetIgnoreJumpInput(false); -> 점프 구현 시 주석 풀기
}

void ACH4PlayerController::Client_PlayDownAnim_Implementation()
{
    ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());
    if (MyChar)
    {
        // MyChar->PlayDownAnimation();
    }
}

void ACH4PlayerController::Client_PlayReviveAnim_Implementation()
{
    ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());
    if (MyChar)
    {
        // MyChar->PlayReviveAnimation();
    }
}

void ACH4PlayerController::Client_MoveToLobby_Implementation()
{
    HUDLobbyWidgetInstance = CreateWidget<UUserWidget>(this,HUDLobbyWidgetClass);
    HUDLobbyWidgetInstance->AddToViewport();
    // 회색 화면 등 -> 구현 필요
}

void ACH4PlayerController::Client_InvokeGameClearUI_Implementation()
{
    HideCurrentWidget();
    
    CurrentWidget = CreateWidget<UUserWidget>(this,HUDGameClearWidgetClass);
    if (CurrentWidget)
    {
        CurrentWidget->AddToViewport();
    }
}

void ACH4PlayerController::Client_InvokeGameLoseUI_Implementation()
{
    HUDGameLoseWidgetInstance = CreateWidget<UUserWidget>(this,HUDGameLoseWidgetClass);
    HUDGameLoseWidgetInstance->AddToViewport();
}

void ACH4PlayerController::Client_SetPlayerDownedUI_Implementation(bool bShow)
{
    if (!IsLocalController())
        return;
    
    if (bShow)
    {
        if (!HUDPlayerDownedWidgetInstance)
        {
            HUDPlayerDownedWidgetInstance = CreateWidget<UUserWidget>(this,HUDPlayerDownedWidgetClass);
            if (HUDPlayerDownedWidgetInstance)
            {
                HUDPlayerDownedWidgetInstance->AddToViewport();
            }
        }
    }
    else
    {
        if (HUDPlayerDownedWidgetInstance)
        {
            HUDPlayerDownedWidgetInstance->RemoveFromParent();
            HUDPlayerDownedWidgetInstance = nullptr;
        }
    }
}
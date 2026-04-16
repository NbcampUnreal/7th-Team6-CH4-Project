#include "CH4PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "CH4_TeamProject/Player/CH4Character.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include <Kismet\KismetSystemLibrary.h>

#include "CH4_TeamProject/Game/CH4GameState.h"
#include "Kismet/GameplayStatics.h"

ACH4PlayerController::ACH4PlayerController()
{
	bReplicates = true;
	bShowMouseCursor = true;
}

FGenericTeamId ACH4PlayerController::GetGenericTeamId() const
{
	return ControlledCharacter ? ControlledCharacter->GetGenericTeamId() : FGenericTeamId();
}

void ACH4PlayerController::BeginPlay()
{
	if (IsLocalController())
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
	
	// TODO: 시작 시 어떤 메뉴를 먼저 띄울지 함수 호출 위치 결정
	// ShowStartMenu();
}

void ACH4PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledCharacter = Cast<ACH4Character>(InPawn);
}

void ACH4PlayerController::OnUnPossess()
{
	Super::OnUnPossess();
	
	ControlledCharacter = nullptr;
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
	CurrentMenuWidget = CreateWidget<UUserWidget>(this, StartMenuClass);

	if (CurrentMenuWidget)
	{
		// 2. 화면에 붙이기
		CurrentMenuWidget->AddToViewport();

		// 3. 마우스 설정: 시작 메뉴에서는 버튼을 눌러야 하니 마우스를 보여줍니다.
		bShowMouseCursor = true;

		// 4. 입력 모드: 게임 캐릭터 조작은 막고 UI만 만지게 설정합니다.
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
	}
}

void ACH4PlayerController::ShowGameOver()
{
	if (GameOverWidgetClass)
	{
		// 1. 게임 오버 위젯 생성
		CurrentGameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);

		if (CurrentGameOverWidget)
		{
			// 2. 화면에 표시
			CurrentGameOverWidget->AddToViewport();

			// 3. 마우스 커서 보이게 설정
			bShowMouseCursor = true;

			// 4. 입력 모드를 UI 전용으로 (그래야 버튼이 눌림)
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(CurrentGameOverWidget->TakeWidget());
			SetInputMode(InputMode);

			//        // 5. 게임 일시정지 (필요하다면)
			//        // UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}
}

void ACH4PlayerController::HideCurrentWidget()
{
	// 1. 만약 현재 떠 있는 메뉴(CurrentMenuWidget)가 있다면?
	if (CurrentMenuWidget)
	{
		// 화면에서 지우고
		CurrentMenuWidget->RemoveFromParent();

		// 메모리 주소도 깨끗하게 비웁니다 (방금 배운 nullptr!)
		CurrentMenuWidget = nullptr;
	}

	// 3. 입력을 다시 '게임 전용'으로 바꿉니다. (그래야 캐릭터가 움직입니다)
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ACH4PlayerController::StartGame()
{
	UE_LOG(LogTemp,Error,TEXT("호출됨?"))
	if (UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport())
	{
		ViewportClient->RemoveAllViewportWidgets();
	}

	if (CurrentMenuWidget)
	{
		CurrentMenuWidget->RemoveFromParent();
		CurrentMenuWidget = nullptr;
	}

	// 3. 게임 화면이 나왔으니 다시 HUD(체력바 등)를 띄워줌
	CurrentMenuWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
	if (HUDWidgetClass)
	{
		CurrentMenuWidget->AddToViewport();

		// 4. ★핵심: 입력 모드를 게임 전용으로! (이걸 안 하면 화면만 비고 조작이 안 됨)
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}

void ACH4PlayerController::BackToLobby_Implementation()
{
	if (!HasAuthority()) return;

	ACH4GameMode* GM = GetWorld()->GetAuthGameMode<ACH4GameMode>();
	if (GM)
	{
		UE_LOG(LogTemp, Warning, TEXT("서버에서 GM을 찾았습니다. 이동 명령을 내립니다!"));
		GM->RequestReturnToLobby();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GM이 유효하지 않음!"));
	}
}

// void ACH4PlayerController::Server_StartGame_Implementation()
// {
//     ACH4GameMode* GM = Cast<ACH4GameMode>(GetWorld()->GetAuthGameMode());
//     GM->PlayGame();
// }

// void ACH4PlayerController::JoinServer()
// {
//     UE_LOG(LogTemp, Warning, TEXT("JoinServer Called"));
//     
//     if (IsLocalController())
//     {
//         ClientTravel(TEXT("127.0.0.1:7777"), TRAVEL_Absolute);
//     }
// }

void ACH4PlayerController::ShowGameClear()
{
	HideCurrentWidget();

	if (GameClearWidgetClass)
	{
		// 위젯 생성
		CurrentGameClearWidget = CreateWidget<UUserWidget>(this, GameClearWidgetClass);

		if (CurrentGameClearWidget)
		{
			// 화면에 띄우기
			CurrentGameClearWidget->AddToViewport();

			// 마우스 커서 활성화
			bShowMouseCursor = true;

			// 입력 모드를 UI 전용으로 (클리어 창의 버튼을 눌러야 하니까요)
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(CurrentGameClearWidget->TakeWidget());
			SetInputMode(InputMode);

			 // [선택] 게임을 일시정지 시키고 싶다면 아래 주석을 푸세요
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
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
	UE_LOG(LogTemp, Warning, TEXT("야후! ShowGameRule 함수가 일단 실행은 됐어!"));
	// 1. 이미 열린 게 있다면 지웁니다.
	HideCurrentWidget();

	// 2. 클래스 체크
	if (!GameRulesWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("위젯이 유효하지않아"))
		return;
	}

	// 3. 룰 위젯 생성 및 저장
	CurrentMenuWidget = CreateWidget<UUserWidget>(this, GameRulesWidgetClass);
	if (CurrentMenuWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("위젯 생성 및 뷰포트 추가 완료!"));
		// 4. 화면에 띄우기 (AddToViewport)
		CurrentMenuWidget->AddToViewport();

		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
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
		MyChar->PlayDownAnimation();
	}
}

void ACH4PlayerController::Client_PlayReviveAnim_Implementation()
{
	ACH4Character* MyChar = Cast<ACH4Character>(GetPawn());
	if (MyChar)
	{
		MyChar->PlayReviveAnimation();
	}
}

void ACH4PlayerController::Client_MoveToLobby_Implementation()
{
	HUDLobbyWidgetInstance = CreateWidget<UUserWidget>(this, HUDLobbyWidgetClass);
	HUDLobbyWidgetInstance->AddToViewport();
	// 회색 화면 등 -> 구현 필요
}

void ACH4PlayerController::Client_InvokeGameClearUI_Implementation()
{
	HideCurrentWidget();

	CurrentWidget = CreateWidget<UUserWidget>(this, HUDGameClearWidgetClass);
	if (CurrentWidget)
	{
		CurrentWidget->AddToViewport();
	}
}

void ACH4PlayerController::Client_InvokeGameLoseUI_Implementation()
{
	HUDGameLoseWidgetInstance = CreateWidget<UUserWidget>(this, HUDGameLoseWidgetClass);
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
			HUDPlayerDownedWidgetInstance = CreateWidget<UUserWidget>(this, HUDPlayerDownedWidgetClass);
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
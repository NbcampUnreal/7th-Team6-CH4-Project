// Fill out your copyright notice in the Description page of Project Settings.


#include "CH4LobbyGameMode.h"
#include "CH4_TeamProject/Game/CH4GameState.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"


ACH4LobbyGameMode::ACH4LobbyGameMode()
{
	bUseSeamlessTravel = false;
}

void ACH4LobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ACH4LobbyGameMode::SetPlayerLifeState(ACH4PlayerController* PC1)
{
	if (!PC1) return;

	// 1. 일단 기본 PlayerState가 있는지 확인
	APlayerState* BasePS = PC1->PlayerState;
    
	if (!BasePS)
	{
		UE_LOG(LogTemp, Error, TEXT("야후! 엔진 기본 PlayerState조차 아직 NULL이야! (타이밍 문제)"));
		return;
	}

	// 2. 야후가 만든 ACH4PlayerState로 캐스팅 시도
	ACH4PlayerState* PS = Cast<ACH4PlayerState>(BasePS);
	if (!PS) 
	{
		UE_LOG(LogTemp, Error, TEXT("야후! PS는 있는데 ACH4PlayerState 타입이 아니야! (설정 문제)"));
		// 현재 설정된 클래스 이름이 뭔지 로그로 찍어보자
		UE_LOG(LogTemp, Warning, TEXT("현재 PS 클래스 이름: %s"), *BasePS->GetClass()->GetName());
		return;
	}

	// 3. 드디어 성공했을 때
	PS->SetPlayerLifeState(EPlayerLifeState::Ready);
	SetReadyPlayerCount();
	UE_LOG(LogTemp, Warning, TEXT("성공적으로 연결됨!"));
}

void ACH4LobbyGameMode::SetReadyPlayerCount()
{
	ReadyPlayerCount++;
	// 1. 월드가 유효한지 확인 (치명적!)
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is NULL!"));
		return;
	}

	// 2. 로그로 현재 카운트 확인
	UE_LOG(LogTemp, Warning, TEXT("현재 준비 인원: %d / %d"), ReadyPlayerCount, MaxPlayerCount);

	if (ReadyPlayerCount >= MaxPlayerCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("모두 준비됨! 맵 이동 시작..."));
		World->ServerTravel(TEXT("/Game/Maps/REALSTAGE"));
	}
}

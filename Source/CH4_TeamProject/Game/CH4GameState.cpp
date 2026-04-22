
#include "CH4GameState.h"
#include "CH4GameInstance.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "EngineUtils.h" 
#include "InterchangeResult.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/DirectionalLight.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Net/UnrealNetwork.h"
#include <Misc\OutputDeviceNull.h>
#include "CH4_TeamProject/Zombie/ZombieSpawnPoint.h"
#include "CH4_TeamProject/Zombie/Controller/MonsterAIController.h"
#include "GameManagers/FinalDefenceManager.h"

ACH4GameState::ACH4GameState()
{
	bReplicates = true;    
	PrimaryActorTick.bCanEverTick = false;
	
	GearPartsCount = 0;
	
	// 기본값 초기화 (예: Score 초기화)
	Score = 0;
	// LevelDuration = 60.0f;
	MaxLevels = 2;
	
	DirectionalLight = nullptr;
	SkyLight = nullptr;
	Fog = nullptr;
	
	FinalDefenceWaveSpawned = false;
}

void ACH4GameState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			ServerTimeHandle,
			this,
			&ACH4GameState::UpdateLapsedTime,
			1.0f,   // 1초 단위
			true);
	}
	
	FindLightAndFog();
}

void ACH4GameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACH4GameState::UpdateLapsedTime()
{
	UE_LOG(LogTemp, Warning, TEXT("DayPhase 경과 시간 : %d"), ElapsedTime);
	
	ACH4GameMode* GM = Cast<ACH4GameMode>(GetWorld()->GetAuthGameMode());
	GM->SetDayPhaseAtServer(DayPhase);
}


void ACH4GameState::AddScore(int32 Amount)
{
	Score += Amount;

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UCH4GameInstance* CH4GameInstance = Cast<UCH4GameInstance>(GameInstance);
		if (CH4GameInstance)
		{
			CH4GameInstance->AddToScore(Amount);
		}
	}
}

int32 ACH4GameState::GetScore() const
{
	return Score; // 혹은 저장된 변수 반환
}

void ACH4GameState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACH4GameState, ServerTime);
	DOREPLIFETIME(ACH4GameState, GamePhase);
	DOREPLIFETIME(ACH4GameState, GearPartsCount);
	DOREPLIFETIME(ACH4GameState, AlivePlayerCount);
	DOREPLIFETIME(ACH4GameState, DayPhase);
}

void ACH4GameState::OnRep_GearPartsCount()
{
	UE_LOG(LogTemp, Warning, TEXT("GearParts Count: %d"), GearPartsCount);
}

void ACH4GameState::OnRep_GamePhase()
{
	UE_LOG(LogTemp, Warning, TEXT("GamePhase Changed: %d"), GamePhase);
	
	ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC) return;
	
	if (GamePhase == EGamePhase::Clear)
	{
		PC->ShowGameClear();
	}
	else if (GamePhase == EGamePhase::Lose)
	{
		PC->ShowGameOver();
	}
	else if (GamePhase == EGamePhase::StartStage)
	{
		PC->StartGame();
	}
	else if (GamePhase == EGamePhase::FinalDefense)
	{
		// PC-> "%f / 60.f 동안 버티며 구조대를 기다리세요!" 라는 UI 띄우는 함수
		// StartFinalDefenceWave();
		UE_LOG(LogTemp, Warning, TEXT("DayPhase Changed: %d"), DayPhase);
	}
}

void ACH4GameState::OnRep_DayPhase()
{
	ApplyDayPhaseChanges(DayPhase);

	ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetWorld()->GetFirstPlayerController());

	if (PC && PC->CurrentMenuWidget)
	{
		FOutputDeviceNull ar;
		FString command = FString::Printf(TEXT("UpdateDayUI %d"), (int32)DayPhase);

		PC->CurrentMenuWidget->CallFunctionByNameWithArguments(*command, ar, NULL, true);

		UE_LOG(LogTemp, Warning, TEXT("C++에서 위젯 UpdateDayUI 호출 완료! 단계: %d"), (int32)DayPhase);
	}
}

void ACH4GameState::OnRep_ServerTime()
{
	// Todo : UI 갱신
}

void ACH4GameState::AddAlivePlayerCount_Implementation()
{
	AlivePlayerCount++;
}

void ACH4GameState::SubtractAlivePlayerCount_Implementation()
{
	AlivePlayerCount--;
}

void ACH4GameState::AddGearPartsCount()
{
	GearPartsCount++;
	UE_LOG(LogTemp, Warning, TEXT("GearPartsCount: %d"), GearPartsCount);
	
	if (GearPartsCount >= 3 && HasAuthority())
	{
		StartFinalDefenceWave();
	}; 
}

bool ACH4GameState::CheckAlivePlayerIsZero()
{
	bool AlivePlayerIsZero;
	if (AlivePlayerCount > 0)
	{
		AlivePlayerIsZero = false;
	}
	else
	{
		AlivePlayerIsZero = true;
		SetGamePhase(EGamePhase::Lose);
		
		ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetWorld()->GetFirstPlayerController());
		PC->Client_InvokeGameLoseUI();
	}
	return AlivePlayerIsZero;
}

void ACH4GameState::SetGamePhase(EGamePhase NewPhase)
{
	if (GamePhase == NewPhase) return;
	GamePhase = NewPhase;
	
	if (HasAuthority() && NewPhase == EGamePhase::FinalDefense)
		StartFinalDefenceWave();
}

void ACH4GameState::ApplyDayPhaseChanges(EDayPhase DP)
{
	if (!DirectionalLight || !SkyLight || !Fog)
	{
		UE_LOG(LogTemp, Warning, TEXT("라이트 가져오기 실패(ApplyDayPhaseChanges 함수)."));
		GetWorldTimerManager().SetTimerForNextTick(this, &ACH4GameState::FindLightAndFog);
		
		return;
	}
 
	switch (DP)
	{
		case EDayPhase::None:
			{
				break;
			}
		
		case EDayPhase::Day:
			{
				UE_LOG(LogTemp, Warning, TEXT("DayPhase : Day"));
				
				DayCount++;
				
				DirectionalLight->SetActorRotation(FRotator(-45.f, 0.f, 0.f)); // 높이
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));
				DirectionalLight->GetLightComponent()->SetIntensity(1.f); // 밝기
		
				SkyLight->GetLightComponent()->SetIntensity(10.f);
				Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.7f, 0.8f, 1.0f));
				
				break;
			}
		
		case EDayPhase::Evening:
			{
				UE_LOG(LogTemp, Warning, TEXT("DayPhase : Evening"));
			
				DirectionalLight->SetActorRotation(FRotator(-10.f, 0.f, 0.f));
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.5f, 0.3f));
				DirectionalLight->GetLightComponent()->SetIntensity(5.f);
				
				SkyLight->GetLightComponent()->SetIntensity(0.6f);				
				Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(1.0f, 0.4f, 0.2f));
				
				// 난이도 변경 함수
	
				break;
			}
		
		case EDayPhase::Night:
			{
				UE_LOG(LogTemp, Warning, TEXT("DayPhase : Night"));
								
				DirectionalLight->SetActorRotation(FRotator(30.f, 0.f, 0.f));
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(0.2f, 0.3f, 0.6f));
				DirectionalLight->GetLightComponent()->SetIntensity(0.05f);
		
				SkyLight->GetLightComponent()->SetIntensity(0.2f);				
				Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.02f, 0.02f, 0.05f));
				
				PlayZombieSound();
				
				break;
			}
	}
	SkyLight->GetLightComponent()->RecaptureSky(); // 바꾼 값 갱신
}

void ACH4GameState::FindLightAndFog()
{
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		DirectionalLight = *It;
		UE_LOG(LogTemp, Warning, TEXT("DirectionalLight Actor is found."));
		
		if (DirectionalLight == nullptr) 
			UE_LOG(LogTemp, Error, TEXT("DirectionalLight Actor is not found."));
		break;
	}
	
	for (TActorIterator<ASkyLight> It(GetWorld()); It; ++It)
	{
		SkyLight = *It;
		UE_LOG(LogTemp, Warning, TEXT("SkyLight Actor is found."));
		
		if (SkyLight == nullptr) 
			UE_LOG(LogTemp, Error, TEXT("SkyLight Actor is not found."));
		break;
	}
	
	for (TActorIterator<AExponentialHeightFog> It(GetWorld()); It; ++It)
	{
		Fog = *It;
		UE_LOG(LogTemp, Warning, TEXT("Fog Actor is found."));
		
		if (Fog == nullptr) 
			UE_LOG(LogTemp, Error, TEXT("Fog Actor is not found."));
		break;
	}
	
	if (DirectionalLight == nullptr || SkyLight == nullptr || Fog == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("날씨 액터 못찾음."));
		GetWorldTimerManager().SetTimerForNextTick(this, &ACH4GameState::FindLightAndFog);
	}
}

void ACH4GameState::PlayZombieSound()
{
	if (!ZombieSound) return;

	UGameplayStatics::PlaySound2D(GetWorld(), ZombieSound);
}

void ACH4GameState::SetDayPhase(EDayPhase NewPhase)
{
	if (DayPhase == NewPhase) return;
	
	UE_LOG(LogTemp, Warning, TEXT("SetDayPhase 호출됨! 이전: %d, 신규: %d"), DayPhase, NewPhase);
	
	DayPhase = NewPhase;
	
	if (HasAuthority()) // 서버에서 따로 실행
	{
		ApplyDayPhaseChanges(NewPhase);
	}
}

void ACH4GameState::UpdateFinalDefenceTimerHandle()
{
	FinalDefenceElapsedTime++;
	UE_LOG(LogTemp, Error, TEXT("FinalDefenceElapsedTime : %d 초"), FinalDefenceElapsedTime)
	
	if (FinalDefenceElapsedTime >= 6 * 60)
	{
		ACH4GameMode* GM = Cast<ACH4GameMode>(GetWorld()->GetAuthGameMode());
		if (!GM) return;
		
		GM->SetGameResult();
		GetWorldTimerManager().ClearTimer(FinalDefenceTimerHandle);
	}
}

void ACH4GameState::StartFinalDefenceWave()
{
	if (!HasAuthority()) return;
	
	UE_LOG(LogTemp, Error, TEXT("FinalDefenceWave() 호출 됨."))

	GetWorldTimerManager().ClearTimer(ServerTimeHandle);
	UE_LOG(LogTemp, Error, TEXT("ServerTimeHandle 꺼짐"))
		
	GetWorldTimerManager().SetTimer(
		FinalDefenceTimerHandle,
		this,
		&ACH4GameState::UpdateFinalDefenceTimerHandle,
		1.0f,   // 1초 단위
		true);
	
	SetDayPhase(EDayPhase::Night); // 밤 상태로 전환(고정)
	
	ACH4GameMode* GM = Cast<ACH4GameMode>(GetWorld()->GetAuthGameMode());
	if (GM) 
		GM->FinalDefenceWaveSpawn(); 
	
	
	if (FinalDefenceWaveSpawned == true)
	{
		TArray<AActor*> ZombieActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieBase::StaticClass(), ZombieActors);
		
		for (AActor* Actor : ZombieActors)
		{
			// AMonsterAIController* AC = Cast<AMonsterAIController>();
			AZombieBase* Zombie = Cast<AZombieBase>(Actor);
			if (!Zombie) continue;
			
			// AI 컨트롤러 캐스팅
			AMonsterAIController* AC = Cast<AMonsterAIController>(Zombie->GetController());
			if (!AC) continue;
		
			AC->SetZombieDetectionRange(); // 좀비 탐지 범위 증가
		}
	}
}

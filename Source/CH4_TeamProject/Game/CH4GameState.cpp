
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
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Net/UnrealNetwork.h"

ACH4GameState::ACH4GameState()
{
	bReplicates = true;
	// PrimaryActorTick.bCanEverTick = false;
	
	GamePhase = EGamePhase::None;
	GearPartsCount = 0;
	
	// 기본값 초기화 (예: Score 초기화)
	Score = 0;
	// LevelDuration = 60.0f;
	MaxLevels = 2;
	
	DirectionalLight = nullptr;
	SkyLight = nullptr;
	Fog = nullptr;
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

void ACH4GameState::UpdateLapsedTime()
{
	// ElapsedTime += 1.f;
	UE_LOG(LogTemp, Warning, TEXT("DayPhase 경과 시간 : %d"), ElapsedTime);
	
	if (ElapsedTime >= TotalDayPhaseCycleTime)
		ElapsedTime = 0;
	
	EDayPhase NewPhase = DayPhase;
	
	if (ElapsedTime < DayTime)
	{
		NewPhase = EDayPhase::Day;
	}
	else if (ElapsedTime < DayTime + EveningTime)
	{
		NewPhase = EDayPhase::Evening;
	}
	else
	{
		NewPhase = EDayPhase::Night;
	}
	
	if (DayPhase != NewPhase)
	{
		DayPhase = NewPhase;
	
		OnRep_DayPhase(); 
		// 서버에서는 OnRep 자동 호출 x -> 서버에서 DayPhaseChang 실행 -> 클라에 변수 자동 복제 -> 클라에서 실행
	}
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

void ACH4GameState::OnRep_GamePhase() // 변경 시 자동 호출
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
}

void ACH4GameState::OnRep_DayPhase()
{
	ApplyDayPhaseChanges(DayPhase);
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
	if (GearPartsCount >= 3)
	{
		if (HasAuthority())
		{
			ACH4GameMode* GM = Cast<ACH4GameMode>(GetWorld()->GetAuthGameMode());
			GM->SetGameResult();
			// GM->BeginFinalDefence();
		}
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
		
				DirectionalLight->SetActorRotation(FRotator(-45.f, 0.f, 0.f)); // 높이
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));
				DirectionalLight->GetLightComponent()->SetIntensity(1.f); // 밝기
		
				SkyLight->GetLightComponent()->SetIntensity(10.f);				
				Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.7f, 0.8f, 1.0f));
				
				// 난이도 변경 함수
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
				// UI알림 : "밤을 대비하세요!"
				break;
			}
		
		case EDayPhase::Night:
			{
				UE_LOG(LogTemp, Warning, TEXT("DayPhase : Night"));
			
				DirectionalLight->SetActorRotation(FRotator(30.f, 0.f, 0.f));
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(0.2f, 0.3f, 0.6f));
				DirectionalLight->GetLightComponent()->SetIntensity(0.2f);
		
				SkyLight->GetLightComponent()->SetIntensity(0.2f);				
				Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.05f, 0.05f, 0.1f));
				
				// 난이도 변경 함수
				// 좀비 비명소리 재생 함수
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

void ACH4GameState::SetDayPhase(EDayPhase NewPhase)
{
	if (DayPhase == NewPhase) return;
	DayPhase = NewPhase;
	
	if (HasAuthority()) // 서버에서 따로 실행
	{
		ApplyDayPhaseChanges(NewPhase);
	}
}

// void ACH4GameState::SetLightsAndFogActor()
// {
// 	if (!GetWorld()) return;
// 	
//
// 	
//
// } 


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
	
	Score = 0;
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

void ACH4GameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACH4GameState::UpdateLapsedTime()
{
	ServerTime++;
	
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
	return Score;
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
}

void ACH4GameState::OnRep_GamePhase()
{
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
	}
}

void ACH4GameState::OnRep_ServerTime()
{
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
				DayCount++;
				
				DirectionalLight->SetActorRotation(FRotator(-45.f, 0.f, 0.f));
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));
				DirectionalLight->GetLightComponent()->SetIntensity(1.f);
		
				SkyLight->GetLightComponent()->SetIntensity(10.f);
				Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.7f, 0.8f, 1.0f));
				
				break;
			}
		
		case EDayPhase::Evening:
			{
				DirectionalLight->SetActorRotation(FRotator(-10.f, 0.f, 0.f));
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.5f, 0.3f));
				DirectionalLight->GetLightComponent()->SetIntensity(5.f);
				
				SkyLight->GetLightComponent()->SetIntensity(0.6f);				
				Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(1.0f, 0.4f, 0.2f));
				
				break;
			}
		
		case EDayPhase::Night:
			{
				DirectionalLight->SetActorRotation(FRotator(30.f, 0.f, 0.f));
				DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(0.2f, 0.3f, 0.6f));
				DirectionalLight->GetLightComponent()->SetIntensity(0.05f);
		
				SkyLight->GetLightComponent()->SetIntensity(0.2f);				
				Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.02f, 0.02f, 0.05f));
				
				PlayZombieSound();
				
				break;
			}
	}
	SkyLight->GetLightComponent()->RecaptureSky();
}

void ACH4GameState::FindLightAndFog()
{
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		DirectionalLight = *It;
		
		if (DirectionalLight == nullptr) 
		break;
	}
	
	for (TActorIterator<ASkyLight> It(GetWorld()); It; ++It)
	{
		SkyLight = *It;
		
		if (SkyLight == nullptr) 
		break;
	}
	
	for (TActorIterator<AExponentialHeightFog> It(GetWorld()); It; ++It)
	{
		Fog = *It;
		
		if (Fog == nullptr) 
		break;
	}
	
	if (DirectionalLight == nullptr || SkyLight == nullptr || Fog == nullptr)
	{
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
	
	DayPhase = NewPhase;
	
	if (HasAuthority())
	{
		ApplyDayPhaseChanges(NewPhase);
	}
}

void ACH4GameState::UpdateFinalDefenceTimerHandle()
{
	FinalDefenceElapsedTime++;
	
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
	
	GetWorldTimerManager().ClearTimer(ServerTimeHandle);
		
	GetWorldTimerManager().SetTimer(
		FinalDefenceTimerHandle,
		this,
		&ACH4GameState::UpdateFinalDefenceTimerHandle,
		1.0f,
		true);
	
	SetDayPhase(EDayPhase::Night);
	
	ACH4GameMode* GM = Cast<ACH4GameMode>(GetWorld()->GetAuthGameMode());
	if (GM) 
		GM->FinalDefenceWaveSpawn(); 
}

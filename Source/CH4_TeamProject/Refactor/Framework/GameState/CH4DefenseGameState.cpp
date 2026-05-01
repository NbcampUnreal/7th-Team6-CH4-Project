#include "CH4DefenseGameState.h"

#include "CH4_TeamProject/Refactor/Framework/GameInstance/CH4DefenseGameInstance.h"
#include "CH4_TeamProject/Refactor/Framework/GameMode/CH4DefenseGameMode.h"
#include "CH4_TeamProject/Refactor/Player/Controller/CH4DefensePlayerController.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "Net/UnrealNetwork.h"

ACH4DefenseGameState::ACH4DefenseGameState()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
}

void ACH4DefenseGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			ServerTimeHandle,
			this,
			&ACH4DefenseGameState::UpdateLapsedTime,
			1.0f,
			true);
	}

	FindLightAndFog();
}

void ACH4DefenseGameState::UpdateLapsedTime()
{
	ServerTime++;

	if (ACH4DefenseGameMode* GM = Cast<ACH4DefenseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->SetDayPhaseAtServer(DayPhase);
	}
}

void ACH4DefenseGameState::AddScore(int32 Amount)
{
	Score += Amount;

	if (UCH4DefenseGameInstance* GI = Cast<UCH4DefenseGameInstance>(GetGameInstance()))
	{
		GI->AddToScore(Amount);
	}
}

int32 ACH4DefenseGameState::GetScore() const
{
	return Score;
}

void ACH4DefenseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4DefenseGameState, ServerTime);
	DOREPLIFETIME(ACH4DefenseGameState, GamePhase);
	DOREPLIFETIME(ACH4DefenseGameState, GearPartsCount);
	DOREPLIFETIME(ACH4DefenseGameState, AlivePlayerCount);
	DOREPLIFETIME(ACH4DefenseGameState, DayPhase);
}

void ACH4DefenseGameState::OnRep_GearPartsCount()
{
}

void ACH4DefenseGameState::OnRep_GamePhase()
{
	ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(GetWorld()->GetFirstPlayerController());
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

void ACH4DefenseGameState::OnRep_DayPhase()
{
	ApplyDayPhaseChanges(DayPhase);

	ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC && PC->CurrentMenuWidget)
	{
		FOutputDeviceNull Ar;
		FString Command = FString::Printf(TEXT("UpdateDayUI %d"), (int32)DayPhase);
		PC->CurrentMenuWidget->CallFunctionByNameWithArguments(*Command, Ar, nullptr, true);
	}
}

void ACH4DefenseGameState::OnRep_ServerTime()
{
}

void ACH4DefenseGameState::AddAlivePlayerCount_Implementation()
{
	AlivePlayerCount++;
}

void ACH4DefenseGameState::SubtractAlivePlayerCount_Implementation()
{
	AlivePlayerCount--;
}

void ACH4DefenseGameState::AddGearPartsCount()
{
	GearPartsCount++;

	if (GearPartsCount >= 3 && HasAuthority())
	{
		StartFinalDefenseWave();
	}
}

bool ACH4DefenseGameState::CheckAlivePlayerIsZero()
{
	if (AlivePlayerCount > 0)
	{
		return false;
	}

	SetGamePhase(EGamePhase::Lose);

	if (ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->Client_InvokeGameLoseUI();
	}
	return true;
}

void ACH4DefenseGameState::SetGamePhase(EGamePhase NewPhase)
{
	if (GamePhase == NewPhase) return;
	GamePhase = NewPhase;

	if (HasAuthority() && NewPhase == EGamePhase::FinalDefense)
	{
		StartFinalDefenseWave();
	}
}

void ACH4DefenseGameState::ApplyDayPhaseChanges(EDayPhase DP)
{
	if (!DirectionalLight || !SkyLight || !Fog)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ACH4DefenseGameState::FindLightAndFog);
		return;
	}

	switch (DP)
	{
	case EDayPhase::None:
		break;

	case EDayPhase::Day:
		DayCount++;
		DirectionalLight->SetActorRotation(FRotator(-45.f, 0.f, 0.f));
		DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.95f, 0.8f));
		DirectionalLight->GetLightComponent()->SetIntensity(1.f);
		SkyLight->GetLightComponent()->SetIntensity(10.f);
		Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.7f, 0.8f, 1.0f));
		break;

	case EDayPhase::Evening:
		DirectionalLight->SetActorRotation(FRotator(-10.f, 0.f, 0.f));
		DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.5f, 0.3f));
		DirectionalLight->GetLightComponent()->SetIntensity(5.f);
		SkyLight->GetLightComponent()->SetIntensity(0.6f);
		Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(1.0f, 0.4f, 0.2f));
		break;

	case EDayPhase::Night:
		DirectionalLight->SetActorRotation(FRotator(30.f, 0.f, 0.f));
		DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor(0.2f, 0.3f, 0.6f));
		DirectionalLight->GetLightComponent()->SetIntensity(0.05f);
		SkyLight->GetLightComponent()->SetIntensity(0.2f);
		Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.02f, 0.02f, 0.05f));
		PlayZombieSound();
		break;
	}

	SkyLight->GetLightComponent()->RecaptureSky();
}

void ACH4DefenseGameState::FindLightAndFog()
{
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		DirectionalLight = *It;
		if (DirectionalLight == nullptr) break;
	}

	for (TActorIterator<ASkyLight> It(GetWorld()); It; ++It)
	{
		SkyLight = *It;
		if (SkyLight == nullptr) break;
	}

	for (TActorIterator<AExponentialHeightFog> It(GetWorld()); It; ++It)
	{
		Fog = *It;
		if (Fog == nullptr) break;
	}

	if (DirectionalLight == nullptr || SkyLight == nullptr || Fog == nullptr)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ACH4DefenseGameState::FindLightAndFog);
	}
}

void ACH4DefenseGameState::PlayZombieSound()
{
	if (!ZombieSound) return;

	UGameplayStatics::PlaySound2D(GetWorld(), ZombieSound);
}

void ACH4DefenseGameState::SetDayPhase(EDayPhase NewPhase)
{
	if (DayPhase == NewPhase) return;

	DayPhase = NewPhase;

	if (HasAuthority())
	{
		ApplyDayPhaseChanges(NewPhase);
	}
}

void ACH4DefenseGameState::UpdateFinalDefenseTimerHandle()
{
	FinalDefenseElapsedTime++;

	if (FinalDefenseElapsedTime >= 6 * 60)
	{
		ACH4DefenseGameMode* GM = Cast<ACH4DefenseGameMode>(GetWorld()->GetAuthGameMode());
		if (!GM) return;

		GM->SetGameResult();
		GetWorldTimerManager().ClearTimer(FinalDefenseTimerHandle);
	}
}

void ACH4DefenseGameState::StartFinalDefenseWave()
{
	if (!HasAuthority()) return;

	GetWorldTimerManager().ClearTimer(ServerTimeHandle);

	GetWorldTimerManager().SetTimer(
		FinalDefenseTimerHandle,
		this,
		&ACH4DefenseGameState::UpdateFinalDefenseTimerHandle,
		1.0f,
		true);

	SetDayPhase(EDayPhase::Night);

	if (ACH4DefenseGameMode* GM = Cast<ACH4DefenseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->FinalDefenseWaveSpawn();
	}
}

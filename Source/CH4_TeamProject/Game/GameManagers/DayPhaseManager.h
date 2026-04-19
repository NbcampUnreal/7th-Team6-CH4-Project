
#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Game/CH4GameMode.h"
#include "DayPhaseManager.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ADayPhaseManager : public ACH4GameMode
{
	GENERATED_BODY()
	
	ADayPhaseManager();
	~ADayPhaseManager();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY()
	float MaxDayPhaseTime;
	
	UPROPERTY()
	float RemainingDayPhaseTime;

	FTimerHandle DayPhaseTimeCountdownHandle;
	
	UPROPERTY()
	float DayTime;
	
	UPROPERTY()
	float EveningTime;
	
	UPROPERTY()
	float NightTime;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnTimerFinished();
	
public:
	UFUNCTION(Server, Reliable)
	void StartDayPhaseTimeCountdown();
	
	UFUNCTION(Server, Reliable)
	void UpdateDayPhaseTimeCountdown();
	
	UFUNCTION(Server, Reliable)
	void Server_SetDayPhaseByTime();
	
	float GetRemainingDayPhaseTime() const { return RemainingDayPhaseTime; }
	
};


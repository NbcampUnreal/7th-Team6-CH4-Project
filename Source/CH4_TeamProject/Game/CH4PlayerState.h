
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4PlayerState.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACH4PlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(ReplicatedUsing = OnRep_LifeState, BlueprintReadOnly)
	EPlayerLifeState LifeState = EPlayerLifeState::Alive;
	
	UPROPERTY(Replicated)
	float CurrentHP;
	
	UPROPERTY(Replicated)
	float MaxHP;
	
	UPROPERTY(Replicated)
	int32 PlayerReviveCount;
	
	UPROPERTY(Replicated)
	int32 SpawnPointIndex; // 배정된 스폰 포인트

	
	UFUNCTION()
	void OnRep_LifeState();

	void SetLifeState(EPlayerLifeState NewState);
	
	UFUNCTION(Server, Reliable)
	void Server_SetCurrentHP(float Damage);
	
	void SetPlayerLifeState(EPlayerLifeState PL);
};

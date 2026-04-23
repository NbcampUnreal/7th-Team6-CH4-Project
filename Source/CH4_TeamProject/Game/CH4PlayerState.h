
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
	int32 PlayerReviveCount;
	
	UPROPERTY(Replicated)
	int32 SpawnPointIndex;

	
	UFUNCTION()
	void OnRep_LifeState();

	void SetLifeState(EPlayerLifeState NewState);
	
	void SetPlayerLifeState(EPlayerLifeState PL);
};

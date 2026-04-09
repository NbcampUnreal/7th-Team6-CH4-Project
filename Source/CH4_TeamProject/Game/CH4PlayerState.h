
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
	
	// 어떤 변수를 복제할지 엔진에 등록하는 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(ReplicatedUsing = OnRep_LifeState, BlueprintReadOnly)
	EPlayerLifeState LifeState = EPlayerLifeState::Alive; // 플레이어 상태(생존)
	
	UPROPERTY(Replicated)
	float CurrentHP;
	
	UPROPERTY(Replicated)
	float MaxHP;
	
	UPROPERTY(Replicated)
	int32 SpawnPointIndex; // 배정된 스폰 포인트
	
	UFUNCTION()
	void OnRep_LifeState();

	void SetLifeState(EPlayerLifeState NewState);
	void SetCurrentHP(float Damage);
	
	UFUNCTION(Server, Reliable)
	void Server_RequestDown();	
	
	// UFUNCTION()
	// void OnRep_IsDowned(); // 다운 연출 (팀원 화면에도 표시)
};

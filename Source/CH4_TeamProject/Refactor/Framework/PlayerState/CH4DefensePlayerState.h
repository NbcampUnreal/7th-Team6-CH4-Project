/*
 * [Refactor] ACH4DefensePlayerState
 *
 * 출처: Game/CH4PlayerState.h/cpp 의 ACH4PlayerState
 * 변경:
 *   - 클래스명 ACH4PlayerState -> ACH4DefensePlayerState (방어전 모드 전용 PlayerState 로 도메인 명시)
 *   - 폴더 위치 Game/ -> Refactor/Framework/PlayerState/
 *   - 사용처 없는 멤버 제거: SpawnPointIndex (UPROPERTY(Replicated) 로 선언했으나
 *     GetLifetimeReplicatedProps 등록 누락 + C++ 어디서도 read/write 없음)
 *   - 중복 메서드 정리: 원본의 SetLifeState, SetPlayerLifeState 둘 다 본문이
 *     `LifeState = X` 한 줄로 동일. SetLifeState 단일 구현으로 통합하고,
 *     SetPlayerLifeState 를 SetLifeState 위임 형태로 유지(BP 호출부 호환)
 *   - 사용처 없는 include 제거 (CH4GameMode.h)
 *   - 본체 동등 (생성자 초기화, GetLifetimeReplicatedProps, OnRep_LifeState 등)
 * 사유: 도메인 명확화 + 폴더 정리 + 데드 코드/중복 메서드 정리
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4DefensePlayerState.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4DefensePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACH4DefensePlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_LifeState, BlueprintReadOnly, Category = "Life")
	EPlayerLifeState LifeState = EPlayerLifeState::Alive;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Life")
	int32 PlayerReviveCount = 2;

	UFUNCTION()
	void OnRep_LifeState();

	void SetLifeState(EPlayerLifeState NewState);

	UFUNCTION(BlueprintCallable, Category = "Life")
	void SetPlayerLifeState(EPlayerLifeState NewState);
};

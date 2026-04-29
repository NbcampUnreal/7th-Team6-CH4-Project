#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4ZombieAIController.generated.h"

class ACH4MonsterBase;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;
struct FAIStimulus;

UCLASS()
class CH4_TEAMPROJECT_API ACH4ZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACH4ZombieAIController();

	virtual void BeginPlay() override;
	virtual FGenericTeamId GetGenericTeamId() const override { return static_cast<uint8>(TeamID); }

	UPROPERTY()
	TObjectPtr<ACH4MonsterBase> ControlledCharacter;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	void ClearSightTarget();
	void ClearHearTarget();
	void ClearAttackTarget();

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	ETeamID TeamID = ETeamID::Monster;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfigComp;

	FTimerHandle LoseSightTargetTimerHandle;
	FTimerHandle LoseAttackTargetTimerHandle;
};

#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Zombie/ZombieBase.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMonsterAIController();
	
	void BeginPlay();

	virtual FGenericTeamId GetGenericTeamId() const override;
	
	UPROPERTY()
	TObjectPtr<AZombieBase> ControlledCharacter;
	
protected:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	void ClearSightTarget();
	void ClearHearTarget();
	void ClearAttackTarget();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	
	FTimerHandle LoseSightTargetTimerHandle;
	FTimerHandle LoseAttackTargetTimerHandle;
};
#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Zombie/ZombieBase.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AISenseConfig_Sight.h"
#include "MonsterAIController.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMonsterAIController();
	
	void BeginPlay();
	
	UPROPERTY()
	TObjectPtr<AZombieBase> ControlledCharacter;
	
	virtual FGenericTeamId GetGenericTeamId() const override { return static_cast<uint8>(TeamID); }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Team")
	ETeamID TeamID = ETeamID::Monster;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfigComp;
	
	FTimerHandle LoseSightTargetTimerHandle;
	FTimerHandle LoseAttackTargetTimerHandle;
};
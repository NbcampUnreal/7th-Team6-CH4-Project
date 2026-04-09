#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMonsterAIController();
	void BeginPlay();

protected:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	virtual void OnPossess(APawn* InPawn) override;
	void ClearTarget();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	
	FTimerHandle LoseSightTimerHandle;
};
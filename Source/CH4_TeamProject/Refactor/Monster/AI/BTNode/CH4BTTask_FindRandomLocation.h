#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CH4BTTask_FindRandomLocation.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4BTTask_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UCH4BTTask_FindRandomLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0.0"))
	float SearchRadius = 1000.0f;
};

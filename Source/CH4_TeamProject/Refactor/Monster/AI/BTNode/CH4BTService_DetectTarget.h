#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CH4BTService_DetectTarget.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4BTService_DetectTarget : public UBTService
{
	GENERATED_BODY()

public:
	UCH4BTService_DetectTarget();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0.0"))
	float DetectRadius = 1000.0f;
};

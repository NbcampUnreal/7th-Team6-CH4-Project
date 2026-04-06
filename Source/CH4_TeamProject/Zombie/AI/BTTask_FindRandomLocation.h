#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomLocation.generated.h"

// UENUM(BlueprintType)
// namespace EBTNodeResult
// {
// 	enum Type : int
// 	{
//      성공
// 		Succeeded,
// 		
//      실패
// 		Failed,
// 		
//      중간에 끊김
// 		Aborted,
// 		
//      진행 중
// 		InProgress,
// 	};
// }

// 반경 내 랜덤한 위치를 Blackboard에 저장하는 Task
UCLASS()
class CH4_TEAMPROJECT_API UBTTask_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindRandomLocation();
	
	// ExecuteTask 함수는 Behavior Tree 내에서 Task가 활성화 되는 순간 호출된다.
	// EBTNodeResult::Type은 현재 실행 중인 Task의 상태를 확인하고 다음 행동을 결정하게 만든다.
	// UBehaviorTreeComponent& OwnerComp는 이 Task를 실행하고 있는 UBehaviorTreeComponent 자체를 가리켜 컨트롤러나 몬스터 본체에 접근할 수 있다.
	// uint8* NodeMemory는 노드에 필요한 메모리 데이터를 가리킨다.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float SearchRadius = 1000.0f;
};
#include "BTTask_FindRandomLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation()
{
	// Behavior Tree 에디터 화면에 표시될 이름을 정함
	NodeName = FString("Find Random Location");
	
	// 현재 이 클래스 내에서 GET_MEMBER_NAME_CHECKED를 이용하여
	// 이 클래스 내에서 똑같은 이름의 변수를 확인 후 Vector 값만 들어오도록 필터를 적용
	PatrolLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindRandomLocation, PatrolLocationKey));
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Behavior Tree를 소유하고 있는 컨트롤러를 AIController 변수에 저장
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr || AIController->GetPawn() == nullptr)
	{
		// Behavior Tree의 실행결과를 나타내는 Enum 값
		return EBTNodeResult::Failed;
	}
	
	// 맵에 있는 UNavigationSystem을 NavSystem 변수에 저장
	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	// AI컨트롤러를 소유하고 있는 폰의 위치를 Origin 변수에 저장
	const FVector Origin = AIController->GetPawn()->GetActorLocation();
	// AI가 실제로 이동할 수 있는 위치를 저장하기 위한 변수를 선언
	FNavLocation RandomLocation;
	
	// 기준점에서 SearchRadius 범위 만큼의 원을 그리고 그 원 안에서 이동할 수 있는 위치를 RandomLocation에 저장
	const bool bFound = NavSystem->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomLocation);
	if (bFound)
	{
		// 위에서 저장한 위치 값을 Blackboard에 기록
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomLocation.Location);
		
		return EBTNodeResult::Succeeded;
	}
	if (!bFound)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
#include "CH4BTTask_FindRandomLocation.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UCH4BTTask_FindRandomLocation::UCH4BTTask_FindRandomLocation()
{
	NodeName = FString("Find Random Location");

	PatrolLocationKey.AddVectorFilter(
		this,
		GET_MEMBER_NAME_CHECKED(UCH4BTTask_FindRandomLocation, PatrolLocationKey));
}

EBTNodeResult::Type UCH4BTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	const UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		return EBTNodeResult::Failed;
	}

	const FVector Origin = AIController->GetPawn()->GetActorLocation();
	FNavLocation RandomLocation;

	if (NavSystem->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(
			PatrolLocationKey.SelectedKeyName,
			RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

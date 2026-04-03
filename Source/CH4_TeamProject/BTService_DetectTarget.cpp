#include "BTService_DetectTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


UBTService_DetectTarget::UBTService_DetectTarget()
{
	NodeName = TEXT("Detect Target");
	
	// 대상 감지 주기
	Interval = 0.5f;
	// Interval의 주기에 편차 추가
	RandomDeviation = 0.1f;

	// 액터들만 골라내 정보를 제한
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DetectTarget, TargetActorKey), AActor::StaticClass());
}

void UBTService_DetectTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr || AIController->GetPawn() == nullptr)
	{
		return;
	}
	
	// 현재 월드 상의 0번 플레이어가 조종하는 Pawn의 정보를 저장
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr)
	{
		// Blackboard에 있는 TargetActorKey에 있던 정보를 삭제
		OwnerComp.GetBlackboardComponent()->ClearValue(TargetActorKey.SelectedKeyName);
		return;
	}
	
	// Dist 함수를 사용해 몬스터와 플레이어 사이의 거리를 구해 Distance 변수에 저장
	const float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());
	
	// 몬스터와 플레이어의 거리가 DetectRadius 범위 안에 있다면 TargetActorKey에 PlayerPawn의 정보를 저장
	if (Distance <= DetectRadius)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetActorKey.SelectedKeyName, PlayerPawn);
	}
	// DetectRadius 범위 밖으로 나가게 되면 TargetActorKey에 있던 정보 삭제
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(TargetActorKey.SelectedKeyName);
	}
}

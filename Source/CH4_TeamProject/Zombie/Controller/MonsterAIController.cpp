#include "MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CH4_TeamProject/Zombie/ZombieBase.h"
#include "Perception/AIPerceptionComponent.h"

AMonsterAIController::AMonsterAIController()
{
	// AI 감지 시스템 이용에 필요한 컴포넌트
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	// AI가 타겟을 감지하거나 놓쳤을 때 OnTargetPerceptionUpdated 함수 호출
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &AMonsterAIController::OnTargetPerceptionUpdated);
	}
}

void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 블랙보트 컴포넌트 정보 저장
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// 감지 범위 안에 들어오면 Blackboard에 타겟 저장
		BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
	}
	else
	{
		// 감지 범위에서 벗어나고 4초 뒤 타겟 정보 삭제
		GetWorld()->GetTimerManager().SetTimer(
			LoseSightTimerHandle,
			this,
			&AMonsterAIController::ClearTarget,
			4.0f,
			false
		);
	}
}

// 컨트롤러가 Pawn에 빙의가 되면 BehaviorTree를 실행하는 함수
void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

// 감지 범위에서 벗어나면 타겟 정보 삭제하는 함수
void AMonsterAIController::ClearTarget()
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->ClearValue(TEXT("TargetActor"));
	}
}

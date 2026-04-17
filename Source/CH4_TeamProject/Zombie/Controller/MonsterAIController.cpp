#include "MonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CH4_TeamProject/Zombie/ZombieBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Hearing.h"

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

FGenericTeamId AMonsterAIController::GetGenericTeamId() const
{
	if (IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetPawn()))
	{
		return TeamAgentInterface->GetGenericTeamId();
	}
	
	return FGenericTeamId();
}

void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 블랙보트 컴포넌트 정보 저장
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	AActor* CurrentTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
	
	if (BlackboardComp == nullptr)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		GetWorld()->GetTimerManager().ClearTimer(LoseSightTimerHandle);
		// 감지 범위 안에 들어오면 Blackboard에 타겟 저장
		BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
	}
	else
	{
		if (Actor == CurrentTarget)
		{
			GetWorld()->GetTimerManager().SetTimer(
				LoseSightTimerHandle,
				this,
				&AMonsterAIController::ClearTarget,
				4.0f,
				false
				);
		}
	}
	
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		// 소리를 들었다 → 소리 위치로 조사
		if (Stimulus.WasSuccessfullySensed())
		{
			GetWorld()->GetTimerManager().ClearTimer(LoseSightTimerHandle);
			GetBlackboardComponent()->SetValueAsVector(
				TEXT("HearingLocation"), Stimulus.StimulusLocation);
		}
		return;
	}
}

// 컨트롤러가 Pawn에 빙의가 되면 BehaviorTree를 실행하는 함수
void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledCharacter = Cast<AZombieBase>(InPawn);
	
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AMonsterAIController::OnUnPossess()
{
	Super::OnUnPossess();
	
	ControlledCharacter = nullptr;
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

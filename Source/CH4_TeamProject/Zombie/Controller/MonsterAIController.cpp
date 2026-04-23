#include "MonsterAIController.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CH4_TeamProject/Zombie/ZombieBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

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
	AActor* CurrentTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));
	
	if (BlackboardComp == nullptr)
	{
		return;
	}
	
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			GetWorld()->GetTimerManager().ClearTimer(LoseSightTargetTimerHandle);
			// 감지 범위 안에 들어오면 Blackboard에 타겟 저장
			BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
		}
		else
		{
			if (Actor == CurrentTarget)
			{
				GetWorld()->GetTimerManager().SetTimer(
					LoseSightTargetTimerHandle,
					this,
					&AMonsterAIController::ClearSightTarget,
					4.0f,
					false
					);
			
				GetWorld()->GetTimerManager().SetTimer(
					LoseAttackTargetTimerHandle,
					this,
					&AMonsterAIController::ClearAttackTarget,
					8.0f,
					false
					);
			}
		}
	}
	
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		// 소리를 감지하면 소리 위치로 이동
		if (Stimulus.WasSuccessfullySensed())
		{
			if (CurrentTarget == nullptr)
			{
				FVector Prev = BlackboardComp->GetValueAsVector(TEXT("HearingLocation"));

				if (FVector::Dist(Prev, Stimulus.StimulusLocation) > 100.0f)
				{
					ClearHearTarget();
					BlackboardComp->SetValueAsVector(TEXT("HearingLocation"), Stimulus.StimulusLocation);
				}
			}
		}
		return;
	}
	
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			GetWorld()->GetTimerManager().ClearTimer(LoseSightTargetTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(LoseAttackTargetTimerHandle);
			
			BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
			BlackboardComp->SetValueAsObject(TEXT("AttackActor"), Actor);
		}
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
void AMonsterAIController::ClearSightTarget()
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->ClearValue(TEXT("TargetActor"));
	}
}

void AMonsterAIController::ClearHearTarget()
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->ClearValue(TEXT("HearingLocation"));
	}
}

void AMonsterAIController::ClearAttackTarget()
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->ClearValue(TEXT("AttackActor"));
	}
}
#include "CH4ZombieAIController.h"

#include "CH4_TeamProject/Refactor/Monster/Character/CH4MonsterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Engine/World.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "TimerManager.h"

ACH4ZombieAIController::ACH4ZombieAIController()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void ACH4ZombieAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &ACH4ZombieAIController::OnTargetPerceptionUpdated);
	}
}

void ACH4ZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCharacter = Cast<ACH4MonsterBase>(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void ACH4ZombieAIController::OnUnPossess()
{
	Super::OnUnPossess();

	ControlledCharacter = nullptr;
}

void ACH4ZombieAIController::ClearSightTarget()
{
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->ClearValue(TEXT("TargetActor"));
	}
}

void ACH4ZombieAIController::ClearHearTarget()
{
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->ClearValue(TEXT("HearingLocation"));
	}
}

void ACH4ZombieAIController::ClearAttackTarget()
{
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->ClearValue(TEXT("AttackActor"));
	}
}

void ACH4ZombieAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!BlackboardComp) return;

	AActor* CurrentTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			GetWorld()->GetTimerManager().ClearTimer(LoseSightTargetTimerHandle);
			BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
		}
		else if (Actor == CurrentTarget)
		{
			GetWorld()->GetTimerManager().SetTimer(
				LoseSightTargetTimerHandle,
				this,
				&ACH4ZombieAIController::ClearSightTarget,
				4.0f,
				false);

			GetWorld()->GetTimerManager().SetTimer(
				LoseAttackTargetTimerHandle,
				this,
				&ACH4ZombieAIController::ClearAttackTarget,
				8.0f,
				false);
		}
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed() && CurrentTarget == nullptr)
		{
			const FVector Prev = BlackboardComp->GetValueAsVector(TEXT("HearingLocation"));

			if (FVector::Dist(Prev, Stimulus.StimulusLocation) > 100.0f)
			{
				ClearHearTarget();
				BlackboardComp->SetValueAsVector(TEXT("HearingLocation"), Stimulus.StimulusLocation);
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

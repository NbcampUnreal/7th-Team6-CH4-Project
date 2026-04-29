#include "CH4BTTask_Attack.h"

#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CH4_TeamProject/Refactor/Monster/Character/CH4MonsterBase.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UCH4BTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/)
{
	bNotifyTick = true;

	AAIController* Owner = OwnerComp.GetAIOwner();
	if (!Owner) return EBTNodeResult::Failed;

	ACH4MonsterBase* Monster = Cast<ACH4MonsterBase>(Owner->GetPawn());
	if (!Monster || !AttackMontage) return EBTNodeResult::Failed;

	Monster->Multicast_PlayAttackMontage(AttackMontage);
	return EBTNodeResult::InProgress;
}

void UCH4BTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* /*NodeMemory*/, float /*DeltaSeconds*/)
{
	AAIController* Owner = OwnerComp.GetAIOwner();
	if (!Owner)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ACharacter* AICharacter = Cast<ACharacter>(Owner->GetPawn());
	if (!AICharacter)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UAnimInstance* AnimInstance = AICharacter->GetMesh() ? AICharacter->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (AnimInstance->Montage_GetIsStopped(AttackMontage))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

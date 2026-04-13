#include "BTTask_Attack.h"
#include "AIController.h"
#include "CH4_TeamProject/Zombie/ZombieBase.h"
#include "GameFramework/Character.h"

UBTTask_Attack::UBTTask_Attack()
{
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bNotifyTick = true;
	
	AAIController* Owner = OwnerComp.GetAIOwner();
	if (Owner == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	// AIController를 소유하고 있는 폰을 ACharacter로 캐스팅
	ACharacter* AIController = Cast<ACharacter>(Owner->GetPawn());
	if (AIController == nullptr || AttackMontage == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	UE_LOG(LogTemp, Error, TEXT("Montage Play!!!!!!!!!!!!!!"))
	AZombieBase* Zombie = Cast<AZombieBase>(Owner->GetPawn());
	if (Zombie)
	{
		Zombie->Multi_PlayAttackMontage(AttackMontage);
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Owner = OwnerComp.GetAIOwner();
	if (Owner == nullptr)
	{
		// FinishLatentTask 함수는 작업이 끝났다고 알려주는 함수
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	ACharacter* AICharacter = Cast<ACharacter>(Owner->GetPawn());
	if (AICharacter == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UAnimInstance* AnimInstance = AICharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		// 여기에서 Failed를 하게되면 Behavior Tree가 멈춰버릴 수 있음
		// AnimInstance는 없지만 이 단계에서 모든 AI 로직이 고장나지 않도록 넘어감
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	// Montage_GetIsStopped 함수는 몽타지가 다 끝났는지 확인하는 함수
	bool bStopped = AnimInstance->Montage_GetIsStopped(AttackMontage);
	if (bStopped)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
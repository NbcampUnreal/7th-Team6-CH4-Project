#include "ZombieAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

// AnimInstance를 사용하는 액터와 이동 컴포넌트를 저장
void UZombieAnimInstance::NativeInitializeAnimation()
{
	// AnimInstance를 사용하는 액터 저장
	OwningCharacter = Cast<AZombieBase>(TryGetPawnOwner());
	if (OwningCharacter == nullptr)
	{
		return;
	}
	
	// OwningCharacter의 이동 컴포넌트 저장
	MovementComponent = OwningCharacter->GetCharacterMovement();
}

// 액터의 현재 상태를 애니메이션에 넘겨준다
void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (MovementComponent == nullptr)
	{
		return;
	}
	
	// 현재 이동속도 저장
	Speed = MovementComponent->Velocity.Size2D();
}
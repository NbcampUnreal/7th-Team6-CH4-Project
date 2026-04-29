#include "CH4MonsterAnimInstance.h"

#include "../Character/CH4MonsterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCH4MonsterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ACH4MonsterBase>(TryGetPawnOwner());
	if (!OwningCharacter) return;

	MovementComponent = OwningCharacter->GetCharacterMovement();
}

void UCH4MonsterAnimInstance::NativeUpdateAnimation(float /*DeltaSeconds*/)
{
	if (!MovementComponent) return;

	Speed = MovementComponent->Velocity.Size2D();
}

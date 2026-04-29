#include "CH4PlayerAnimInstance.h"

#include "../Component/CH4HealthComponent.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UCH4PlayerAnimInstance::PlayActionMontage(ECH4PlayerActionState NewAction)
{
	const FCH4ActionMontageEntry* Entry = ActionMontages.Find(NewAction);
	if (!Entry) return false;

	return TryPlayMontage(Entry->Montage, NewAction, Entry->bStopOtherMontages);
}

void UCH4PlayerAnimInstance::SetDownState(bool bNewIsDown)
{
	bIsDown = bNewIsDown;
}

void UCH4PlayerAnimInstance::SetDeadState(bool bNewIsDead)
{
	bIsDead = bNewIsDead;
}

void UCH4PlayerAnimInstance::ClearActionState()
{
	if (!bIsDead && !bIsDown)
	{
		CurrentActionState = ECH4PlayerActionState::None;
	}
}

void UCH4PlayerAnimInstance::AnimNotify_ClearAction()
{
	ClearActionState();
}

void UCH4PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UCH4PlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Owner)
	{
		Owner = Cast<ACharacter>(GetOwningActor());
	}

	if (Owner && !Movement)
	{
		Movement = Owner->GetCharacterMovement();
	}

	if (Movement)
	{
		Velocity = Movement->Velocity;
		MoveSpeed = Velocity.Size2D();
		bIsFalling = Movement->IsFalling();
	}

	if (Owner)
	{
		if (const UCH4HealthComponent* Health = Owner->FindComponentByClass<UCH4HealthComponent>())
		{
			bIsDown = Health->IsDowned();
		}
	}
}

bool UCH4PlayerAnimInstance::CanPlayAction(ECH4PlayerActionState NewAction) const
{
	if (bIsDead)
	{
		return NewAction == ECH4PlayerActionState::Dead;
	}

	if (bIsDown)
	{
		return (NewAction == ECH4PlayerActionState::Revive || NewAction == ECH4PlayerActionState::Dead);
	}

	switch (CurrentActionState)
	{
	case ECH4PlayerActionState::None:
		return true;

	case ECH4PlayerActionState::Hit:
	case ECH4PlayerActionState::Pickup:
		return (NewAction == ECH4PlayerActionState::Dead || NewAction == ECH4PlayerActionState::Down);

	case ECH4PlayerActionState::PistolFire:
	case ECH4PlayerActionState::RifleFire:
	case ECH4PlayerActionState::ShotgunFire:
		return (NewAction == ECH4PlayerActionState::PistolFire ||
			NewAction == ECH4PlayerActionState::RifleFire ||
			NewAction == ECH4PlayerActionState::ShotgunFire ||
			NewAction == ECH4PlayerActionState::PistolReload ||
			NewAction == ECH4PlayerActionState::RifleReload ||
			NewAction == ECH4PlayerActionState::ShotgunReload ||
			NewAction == ECH4PlayerActionState::Dead ||
			NewAction == ECH4PlayerActionState::Down ||
			NewAction == ECH4PlayerActionState::Hit);

	case ECH4PlayerActionState::PistolReload:
	case ECH4PlayerActionState::RifleReload:
	case ECH4PlayerActionState::ShotgunReload:
		return (NewAction == ECH4PlayerActionState::Dead ||
			NewAction == ECH4PlayerActionState::Down ||
			NewAction == ECH4PlayerActionState::Hit);

	case ECH4PlayerActionState::Down:
		return (NewAction == ECH4PlayerActionState::Dead || NewAction == ECH4PlayerActionState::Revive);

	case ECH4PlayerActionState::Revive:
		return NewAction == ECH4PlayerActionState::Dead;

	case ECH4PlayerActionState::Dead:
		return false;

	default:
		return true;
	}
}

bool UCH4PlayerAnimInstance::TryPlayMontage(UAnimMontage* Montage, ECH4PlayerActionState NewAction, bool bStopOtherMontages)
{
	if (!Montage) return false;
	if (!CanPlayAction(NewAction)) return true;
	if (Montage_IsPlaying(Montage)) return false;

	if (bStopOtherMontages)
	{
		Montage_Stop(0.15f);
	}

	const float PlayResult = Montage_Play(Montage);
	if (PlayResult > 0.0f)
	{
		CurrentActionState = NewAction;
		return true;
	}

	return false;
}

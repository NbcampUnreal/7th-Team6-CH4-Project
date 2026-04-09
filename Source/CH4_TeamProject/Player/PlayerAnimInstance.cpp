#include "CH4_TeamProject/Player/PlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"

//생성자
UPlayerAnimInstance::UPlayerAnimInstance()
{	
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());

	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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
}

bool UPlayerAnimInstance::CanPlayAction(EPlayerActionState NewAction) const
{
	//죽은 상태면 다른 행동 불가, 사망 몽타주는 재생 허용
	if (bIsDead)
	{
		return NewAction == EPlayerActionState::Dead;
	}
	//다운 상태면 소생, 죽음 허용, 줍거나 피격 몽타지는 차단
	if (bIsDown)
	{
		return (NewAction == EPlayerActionState::Revive || NewAction == EPlayerActionState::Dead);
	}

	switch (CurrentActionState)
	{
	case EPlayerActionState::None:
		
		return true;

	case EPlayerActionState::Hit:
		
		return (NewAction == EPlayerActionState::Dead || NewAction == EPlayerActionState::Down);

	case EPlayerActionState::Pickup:
		
		return (NewAction == EPlayerActionState::Dead || NewAction == EPlayerActionState::Down);

	case EPlayerActionState::Down:
		
		return (NewAction == EPlayerActionState::Dead || NewAction == EPlayerActionState::Revive);

	case EPlayerActionState::Revive:
		
		return (NewAction == EPlayerActionState::Dead);

	case EPlayerActionState::Dead:
		
		return false;

	default:
		return true;
	}
}

bool UPlayerAnimInstance::TryPlayMontage(UAnimMontage* Montage, EPlayerActionState NewAction, bool bStopOtherMontages)
{
	// 몽타주 없으면 재생 불가
	if (!Montage)
	{
		return false;
	}

	//현 상태에서 재생가능한지 확인
	if (!CanPlayAction(NewAction))
	{
		return false;
	}

	//몽타주 중복재생 막기
	if (Montage_IsPlaying(Montage))
	{
		return false;
	}

	//우선순위 높은 액션이면 현재 재생 중인 다른 몽타주 중단
	if (bStopOtherMontages)
	{
		Montage_Stop(0.15f);
	}

	//실제 몽타주 재생
	const float PlayResult = Montage_Play(Montage);

	//재생 성공 시 현재 액션 상태 갱신
	if (PlayResult > 0.0f)
	{
		CurrentActionState = NewAction;
		return true;
	}

	//재생 실패
	return false;
}

bool UPlayerAnimInstance::PlayHitHeadMontage()
{
	//피격은 보통 다른 행동을 강제로 끊지는 않음
	return TryPlayMontage(HitHeadMontage, EPlayerActionState::Hit, false);
}

bool UPlayerAnimInstance::PlayPickupMontage()
{
	//줍기도 보통은 강제 인터럽트 없이 재생
	return TryPlayMontage(PickupMontage, EPlayerActionState::Pickup, false);
}

bool UPlayerAnimInstance::PlayDownMontage()
{
	//다운은 다른 몽타주를 끊고 재생
	return TryPlayMontage(DownMontage, EPlayerActionState::Down, true);
}

bool UPlayerAnimInstance::PlayDeathMontage()
{
	//사망은 기존 몽타주 정리 후 재생
	return TryPlayMontage(DeathMontage, EPlayerActionState::Dead, true);
}

bool UPlayerAnimInstance::PlayReviveMontage()
{
	//부활도 기존 몽타주 중지 후 재생
	return TryPlayMontage(ReviveMontage, EPlayerActionState::Revive, true);
}

void UPlayerAnimInstance::SetDownState(bool bNewIsDown)
{
	//다운 유지 상태
	bIsDown = bNewIsDown;
}

void UPlayerAnimInstance::SetDeadState(bool bNewIsDead)
{
	//사망 유지 상태
	bIsDead = bNewIsDead;
}

void UPlayerAnimInstance::ClearActionState()
{
	//죽은 상태,다운 상태가 아닐 때만 액션 초기화
	//다운/죽음은 상태 유지가 필요하므로 None 으로 하지않음
	if (!bIsDead && !bIsDown)
	{
		CurrentActionState = EPlayerActionState::None;
	}
}
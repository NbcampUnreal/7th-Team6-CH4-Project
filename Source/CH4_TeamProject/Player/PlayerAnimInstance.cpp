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

		//발사 중 상태
	case EPlayerActionState::PistolFire:
	case EPlayerActionState::RifleFire:
	case EPlayerActionState::ShotgunFire:
		
		//발사 중에는 같은 발사,죽음,다운,피격 허용
		return (NewAction == EPlayerActionState::PistolFire ||
			NewAction == EPlayerActionState::RifleFire ||
			NewAction == EPlayerActionState::ShotgunFire ||
			NewAction == EPlayerActionState::Dead ||
			NewAction == EPlayerActionState::Down ||
			NewAction == EPlayerActionState::Hit);

		//장전 중 상태
	case EPlayerActionState::PistolReload:
	case EPlayerActionState::RifleReload:
	case EPlayerActionState::ShotgunReload:
		// 장전 중에는 죽음,다운,피격 허용
		return (NewAction == EPlayerActionState::Dead ||
			NewAction == EPlayerActionState::Down ||
			NewAction == EPlayerActionState::Hit);
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
		UE_LOG(LogTemp, Error, TEXT("TryPlayMontage 실패 - Montage가 NULL, NewAction: %d"), (int32)NewAction); // [추가]
		return false;
	}

	//현 상태에서 재생가능한지 확인
	if (!CanPlayAction(NewAction))
	{
		UE_LOG(LogTemp, Error, TEXT("TryPlayMontage 실패 - CanPlayAction 막힘, CurrentActionState: %d, NewAction: %d"),
			(int32)CurrentActionState, (int32)NewAction); // [추가]
		return true;
	}

	//몽타주 중복재생 막기
	if (Montage_IsPlaying(Montage))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryPlayMontage 실패 - 이미 재생 중인 몽타주, NewAction: %d"), (int32)NewAction); // [추가]
		return false;
	}

	//우선순위 높은 액션이면 현재 재생 중인 다른 몽타주 중단
	if (bStopOtherMontages)
	{
		Montage_Stop(0.15f);
	}

	//실제 몽타주 재생
	const float PlayResult = Montage_Play(Montage);
	UE_LOG(LogTemp, Warning, TEXT("Montage_Play 결과: %f, NewAction: %d"), PlayResult, (int32)NewAction); // [추가]

	//재생 성공 시 현재 액션 상태 갱신
	if (PlayResult > 0.0f)
	{
		CurrentActionState = NewAction;
		UE_LOG(LogTemp, Warning, TEXT("TryPlayMontage 성공 - CurrentActionState 변경: %d"), (int32)CurrentActionState); // [추가]
		return true;
	}

	//재생 실패
	UE_LOG(LogTemp, Error, TEXT("TryPlayMontage 실패 - Montage_Play 결과가 0 이하, NewAction: %d"), (int32)NewAction); // [추가]
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

bool UPlayerAnimInstance::PlayPistolFireMontage()
{
	//권총 발사는 보통 다른 행동을 강제로 끊지는 않음
	return TryPlayMontage(PistolFireMontage, EPlayerActionState::PistolFire, false);
}

bool UPlayerAnimInstance::PlayRifleFireMontage()
{
	//라이플 발사는 보통 다른 행동을 강제로 끊지는 않음
	return TryPlayMontage(RifleFireMontage, EPlayerActionState::RifleFire, false);
}

bool UPlayerAnimInstance::PlayPistolReloadMontage()
{
	//권총 장전은 기존 몽타주를 끊고 재생
	return TryPlayMontage(PistolReloadMontage, EPlayerActionState::PistolReload, true);
}

bool UPlayerAnimInstance::PlayRifleReloadMontage()
{
	//라이플 장전은 기존 몽타주를 끊고 재생
	return TryPlayMontage(RifleReloadMontage, EPlayerActionState::RifleReload, true);
}

bool UPlayerAnimInstance::PlayMeleeAttackMontage()
{
	// 근접 공격 몽타주 재생
	return TryPlayMontage(MeleeAttackMontage, EPlayerActionState::MeleeAttack, true);
}

bool UPlayerAnimInstance::PlayDrinkHealItemMontage()
{
	// 체력 회복 아이템 사용하기 몽타주
	return TryPlayMontage(DrinkHealItemMontage, EPlayerActionState::DrinkHealItem, true);
bool UPlayerAnimInstance::PlayShotgunFireMontage()
{
	//샷건 발사는 다른 행동을 강제로 끊지는 않음
	return TryPlayMontage(ShotgunFireMontage, EPlayerActionState::ShotgunFire, false);
}

bool UPlayerAnimInstance::PlayShotgunReloadMontage()
{
	//샷건 장전은 기존 몽타주를 끊고 재생
	return TryPlayMontage(ShotgunReloadMontage, EPlayerActionState::ShotgunReload, true);
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
#include "CH4HealthComponent.h"

#include "CH4MovementComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UCH4HealthComponent::UCH4HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UCH4HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CurrentHP <= 0.0f)
	{
		CurrentHP = MaxHP;
	}
}

void UCH4HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCH4HealthComponent, MaxHP);
	DOREPLIFETIME(UCH4HealthComponent, CurrentHP);
	DOREPLIFETIME(UCH4HealthComponent, bIsDowned);
}

float UCH4HealthComponent::ApplyDamage(float DamageAmount, const FDamageEvent& /*DamageEvent*/, AController* /*EventInstigator*/, AActor* /*DamageCauser*/)
{
	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return 0.0f;

	if (!OwnerChar->HasAuthority())
	{
		return DamageAmount;
	}

	const float PreviousHP = CurrentHP;
	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.0f, MaxHP);
	OnHealthChanged.Broadcast(CurrentHP);

	if (CurrentHP <= 0.0f && !bIsDowned)
	{
		bIsDowned = true;
		HandleDowned();
		OnPlayerDowned.Broadcast();
	}

	return PreviousHP - CurrentHP;
}

void UCH4HealthComponent::Heal(float Amount)
{
	const ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->HasAuthority()) return;

	if (Amount <= 0.0f) return;

	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.0f, MaxHP);
	OnHealthChanged.Broadcast(CurrentHP);
}

void UCH4HealthComponent::Revive(float HealAmount)
{
	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->HasAuthority()) return;

	bIsDowned = false;
	CurrentHP = FMath::Clamp(HealAmount, 1.0f, MaxHP);
	OnHealthChanged.Broadcast(CurrentHP);

	if (UCharacterMovementComponent* Movement = OwnerChar->GetCharacterMovement())
	{
		Movement->SetMovementMode(MOVE_Walking);
		if (UCH4MovementComponent* CH4Move = Cast<UCH4MovementComponent>(Movement))
		{
			Movement->MaxWalkSpeed = CH4Move->DefaultWalkSpeed;
		}
	}
}

void UCH4HealthComponent::OnRep_IsDowned()
{
	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return;

	UCharacterMovementComponent* Movement = OwnerChar->GetCharacterMovement();
	if (!Movement) return;

	if (bIsDowned)
	{
		Movement->MaxWalkSpeed = 0.0f;
	}
	else
	{
		// 소생 시 클라이언트 측 이동 속도 복구.
		if (UCH4MovementComponent* CH4Move = Cast<UCH4MovementComponent>(Movement))
		{
			Movement->MaxWalkSpeed = CH4Move->DefaultWalkSpeed;
		}
	}
}

void UCH4HealthComponent::HandleDowned()
{
	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return;

	if (UCharacterMovementComponent* Movement = OwnerChar->GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 0.0f;
		Movement->DisableMovement();
	}
}

void UCH4HealthComponent::HandleDeath()
{
	OnPlayerDied.Broadcast();
}

ACharacter* UCH4HealthComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

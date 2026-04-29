#include "CH4RunnerZombie.h"

ACH4RunnerZombie::ACH4RunnerZombie()
{
	MaxHP = 80.0f;
	CurrentHP = MaxHP;
	MoveSpeed = 400.0f;
	Damage = 15.0f;
}

float ACH4RunnerZombie::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}


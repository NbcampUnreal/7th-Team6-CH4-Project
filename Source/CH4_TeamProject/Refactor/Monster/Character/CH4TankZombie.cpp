#include "CH4TankZombie.h"

ACH4TankZombie::ACH4TankZombie()
{
	MaxHP = 300.0f;
	CurrentHP = MaxHP;
	MoveSpeed = 150.0f;
	Damage = 35.0f;
}

float ACH4TankZombie::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}


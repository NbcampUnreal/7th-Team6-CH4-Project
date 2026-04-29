#include "CH4NormalZombie.h"

ACH4NormalZombie::ACH4NormalZombie()
{
	MaxHP = 100.0f;
	CurrentHP = MaxHP;
	MoveSpeed = 200.0f;
	Damage = 20.0f;
}

float ACH4NormalZombie::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}


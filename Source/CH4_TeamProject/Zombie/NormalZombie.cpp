#include "NormalZombie.h"

ANormalZombie::ANormalZombie()
{
	MaxHP = 100;
	CurrentHP = MaxHP;
	MoveSpeed = 200.f;
	Damage = 20.f;
}

void ANormalZombie::BeginPlay()
{
	Super::BeginPlay();
}

float ANormalZombie::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 체력이 0보다 작아지면 OnDeath 함수 실행
	if (CurrentHP <= 0)
	{
		OnDeath();
	}
	
	return ActualDamage;
}

void ANormalZombie::OnDeath()
{
	Super::OnDeath();
}

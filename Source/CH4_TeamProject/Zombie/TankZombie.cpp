#include "TankZombie.h"

ATankZombie::ATankZombie()
{
	MaxHP = 300;
	CurrentHP = MaxHP;
	MoveSpeed = 150.f;
	Damage = 35.f;
}

void ATankZombie::BeginPlay()
{
	Super::BeginPlay();
}

float ATankZombie::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 체력이 0보다 작아지면 OnDeath 함수 실행
	if (CurrentHP <= 0)
	{
		OnDeath();
	}
	
	return ActualDamage;
}

void ATankZombie::OnDeath()
{
	Super::OnDeath();
}

void ATankZombie::SetZombieStat(float NewHP, float NewDamage, float NewSpeed)
{
	Super::SetZombieStat(NewHP, NewDamage, NewSpeed);
}

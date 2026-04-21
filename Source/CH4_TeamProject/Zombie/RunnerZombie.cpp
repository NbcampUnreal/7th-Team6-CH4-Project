#include "RunnerZombie.h"

ARunnerZombie::ARunnerZombie()
{
	MaxHP = 80;
	CurrentHP = MaxHP;
	MoveSpeed = 400.f;
	Damage = 15.f;
}

void ARunnerZombie::BeginPlay()
{
	Super::BeginPlay();
}

float ARunnerZombie::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 체력이 0보다 작아지면 OnDeath 함수 실행
	if (CurrentHP <= 0)
	{
		OnDeath();
	}
	
	return ActualDamage;
}

void ARunnerZombie::OnDeath()
{
	Super::OnDeath();
}

void ARunnerZombie::SetZombieStat(float NewHP, float NewDamage, float NewSpeed)
{
	Super::SetZombieStat(NewHP, NewDamage, NewSpeed);
}

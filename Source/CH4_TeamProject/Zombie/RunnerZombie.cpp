#include "RunnerZombie.h"

ARunnerZombie::ARunnerZombie()
{
	MaxHP = 100;
	CurrentHP = MaxHP;
	MoveSpeed = 200.f;
	Damage = 30.f;
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

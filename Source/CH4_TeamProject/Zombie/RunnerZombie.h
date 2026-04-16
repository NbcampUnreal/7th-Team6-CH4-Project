#pragma once

#include "CoreMinimal.h"
#include "ZombieBase.h"
#include "RunnerZombie.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ARunnerZombie : public AZombieBase
{
	GENERATED_BODY()
	
public:
	ARunnerZombie();

protected:
	void BeginPlay();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDeath() override;
};

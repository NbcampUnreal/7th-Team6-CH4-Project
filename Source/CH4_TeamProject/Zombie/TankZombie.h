#pragma once

#include "CoreMinimal.h"
#include "ZombieBase.h"
#include "TankZombie.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ATankZombie : public AZombieBase
{
	GENERATED_BODY()
	
public:
	ATankZombie();

protected:
	void BeginPlay();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDeath() override;
	
};

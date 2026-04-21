#pragma once

#include "CoreMinimal.h"
#include "ZombieBase.h"
#include "NormalZombie.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ANormalZombie : public AZombieBase
{
	GENERATED_BODY()
	
public:
	ANormalZombie();

protected:
	void BeginPlay();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDeath() override;
	virtual void SetZombieStat(float NewHP, float NewDamage, float NewSpeed) override;
};
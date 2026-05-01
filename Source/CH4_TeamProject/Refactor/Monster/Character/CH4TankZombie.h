#pragma once

#include "CoreMinimal.h"
#include "CH4MonsterBase.h"
#include "CH4TankZombie.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4TankZombie : public ACH4MonsterBase
{
	GENERATED_BODY()

public:
	ACH4TankZombie();

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
};

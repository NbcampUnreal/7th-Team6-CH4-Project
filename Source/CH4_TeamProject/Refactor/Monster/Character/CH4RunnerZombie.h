#pragma once

#include "CoreMinimal.h"
#include "CH4MonsterBase.h"
#include "CH4RunnerZombie.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4RunnerZombie : public ACH4MonsterBase
{
	GENERATED_BODY()

public:
	ACH4RunnerZombie();

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
};

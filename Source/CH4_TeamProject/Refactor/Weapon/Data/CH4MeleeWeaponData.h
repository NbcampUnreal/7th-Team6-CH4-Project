#pragma once

#include "CoreMinimal.h"
#include "CH4WeaponData.h"
#include "CH4MeleeWeaponData.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4MeleeWeaponData : public UCH4WeaponData
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditAnywhere, Category = "MeleeWeapon", meta = (ClampMin = "0.0"))
	float AttackRadius = 0.0f;

	UPROPERTY(EditAnywhere, Category = "MeleeWeapon", meta = (ClampMin = "0"))
	int32 AttackDelay = 0;

	UPROPERTY(EditAnywhere, Category = "MeleeWeapon", meta = (ClampMin = "0.0"))
	float KnockbackForce = 0.0f;
};

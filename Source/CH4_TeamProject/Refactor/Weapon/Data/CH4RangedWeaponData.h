#pragma once

#include "CoreMinimal.h"
#include "CH4WeaponData.h"
#include "CH4RangedWeaponData.generated.h"

class ACH4RangedWeapon;

UCLASS()
class CH4_TEAMPROJECT_API UCH4RangedWeaponData : public UCH4WeaponData
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	virtual int32 GetMaxAmmo() const override { return MaxAmmo; }
	virtual float GetRangedLength() const override { return RangedLength; }
	virtual float GetFireRate() const override { return FireRate; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RangedWeapon")
	TSubclassOf<ACH4RangedWeapon> GunWeaponClass;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RangedWeapon", meta = (AllowPrivateAccess = true, ClampMin = "0"))
	int32 MaxAmmo = 300;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RangedWeapon", meta = (AllowPrivateAccess = true, ClampMin = "0.0"))
	float RangedLength = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RangedWeapon", meta = (AllowPrivateAccess = true, ClampMin = "0.0"))
	float FireRate = 0.0f;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4WeaponData.generated.h"

class ACH4ItemBase;
class UNiagaraSystem;
class USoundBase;
class UStaticMesh;

UCLASS()
class CH4_TEAMPROJECT_API UCH4WeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	virtual int32 GetMaxAmmo() const { return 0; }
	virtual float GetRangedLength() const { return 0.0f; }
	virtual float GetFireRate() const { return 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Animation")
	ECombatPose GetCombatPose() const { return CombatPose; }

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetCombatPose(ECombatPose NewCombatPose) { CombatPose = NewCombatPose; }

	UPROPERTY(EditAnywhere, Category = "Common")
	TSubclassOf<ACH4ItemBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon|Visual")
	TObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Common", meta = (ClampMin = "0"))
	int32 Damage = 0;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> MuzzleFlash;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	ECombatPose CombatPose = ECombatPose::Rifle;
};

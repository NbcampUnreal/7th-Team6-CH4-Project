// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/PrimaryAssetId.h"
#include "Engine/DataAsset.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "../WeaponData.h"
#include "RangedWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API URangedGunDataAsset : public UWeaponData
{
	GENERATED_BODY()
private:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon",meta=(AllowPrivateAccess=true))
	int32 MaxAmmo = 300;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon",meta=(AllowPrivateAccess=true))
	float RangedLength = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon",meta=(AllowPrivateAccess=true))
	float FireRate = 0;
public:	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
	virtual int32 GetMaxAmmo() const override { return MaxAmmo; }
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	TSubclassOf<class ARangedWeapons> WeaponClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	ECombatPose CombatPose = ECombatPose::Rifle;
	virtual float GetRangedLength() const override { return RangedLength; }
	
	virtual float GetFireRate() const override { return FireRate; }
	
	
};
	 
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/PrimaryAssetId.h"
#include "Engine/DataAsset.h"
#include "RangedWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API URangedWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	int32 Damage;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	int32 MaxAmmo;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	TSubclassOf<class ARangedWeapons> WeaponClass;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};

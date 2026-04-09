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
class CH4_TEAMPROJECT_API URangedGunDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	int32 Damage = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	int32 MaxAmmo = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	float RangedLength = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	float FireRate = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	TObjectPtr<UStaticMesh> WeaponMesh;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Equip_Gun_Weapon")
	TSubclassOf<class ARangedWeapons> WeaponClass;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
	 
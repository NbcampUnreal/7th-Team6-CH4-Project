// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../WeaponData.h"
#include "MeelWeaponData.generated.h"

/**
 * 
 */

UCLASS()
class CH4_TEAMPROJECT_API UMeleeWeaponData : public UWeaponData
{
	GENERATED_BODY()
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
public:

	//근접 공격 범
	UPROPERTY(EditAnywhere, Category = "Equip_Meel_Weapom")
	float AttackRadius = 0;
	
	//공속
	UPROPERTY(EditAnywhere, Category = "Equip_Meel_Weapom")
	int32 AttackDelay = 0;
	
	//넉백
	UPROPERTY(EditAnywhere, Category = "Equip_Meel_Weapom")
	float KnockbackForce;
};

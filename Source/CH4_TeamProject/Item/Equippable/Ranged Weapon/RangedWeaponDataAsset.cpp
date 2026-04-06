// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeaponDataAsset.h"


FPrimaryAssetId URangedGunDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Weapon"), GetFName());
}

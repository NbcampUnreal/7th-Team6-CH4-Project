// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponData.h"
FPrimaryAssetId UWeaponData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("AllWeapon"), GetFName());
}

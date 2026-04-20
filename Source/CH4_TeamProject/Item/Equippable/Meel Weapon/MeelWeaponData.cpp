// Fill out your copyright notice in the Description page of Project Settings.


#include "MeelWeaponData.h"
FPrimaryAssetId UMeleeWeaponData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("MeelWeapon"), GetFName());
}

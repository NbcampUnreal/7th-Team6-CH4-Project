#include "CH4MeleeWeaponData.h"

FPrimaryAssetId UCH4MeleeWeaponData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("MeleeWeapon"), GetFName());
}

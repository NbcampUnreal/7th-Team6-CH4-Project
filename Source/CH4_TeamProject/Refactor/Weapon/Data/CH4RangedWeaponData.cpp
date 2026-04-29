#include "CH4RangedWeaponData.h"

FPrimaryAssetId UCH4RangedWeaponData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Weapon"), GetFName());
}

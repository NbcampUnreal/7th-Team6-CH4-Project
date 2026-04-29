#include "CH4WeaponData.h"

FPrimaryAssetId UCH4WeaponData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("AllWeapon"), GetFName());
}

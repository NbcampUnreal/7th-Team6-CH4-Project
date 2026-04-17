#pragma once


UENUM(BlueprintType)
enum class EEffectType : uint8
{
	Health UMETA(DisplayName = "Health"),
	Ammo UMETA(DisplayName = "Ammo"),
	Gear UMETA(DisplayName = "Gear"),
};
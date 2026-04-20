
#pragma once
#include "CoreMinimal.h"
#include "DataBase.generated.h"



UENUM(BlueprintType)
enum class EPlayerLifeState : uint8
{
	Ready UMETA(DisplayName = "Ready"),
	Alive UMETA(DisplayName = "Alive"), // 에디터에서 표시할 이름
	Downed UMETA(DisplayName = "Downed"),
	Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	None UMETA(DisplayName = "None"),
	StartStage UMETA(DisplayName = "StartStage"),
	FinalDefense UMETA(DisplayName = "FinalDefense"),
	Clear UMETA(DisplayName = "Clear"),
	Lose UMETA(DisplayName = "Lose")
};

UENUM(BlueprintType)
enum class ETeamID : uint8
{
	Player = 0,
	Monster = 1,
	NoTeam = 255
};

UENUM(BlueprintType)
enum class EDayPhase : uint8
{
	None UMETA(DisplayName = "None"),
	Day UMETA(DisplayName = "DayPhase"),
	Evening UMETA(DisplayName = "Evening"),
	Night UMETA(DisplayName = "Night")
};

UENUM(BlueprintType)
enum class ECombatPose : uint8
{
	Normal  UMETA(DisplayName = "Normal"),
	Melee   UMETA(DisplayName = "Melee"),
	Pistol  UMETA(DisplayName = "Pistol"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Rifle   UMETA(DisplayName = "Rifle")
};



#pragma once

UENUM(BlueprintType)
enum class EPlayerLifeState : uint8
{
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
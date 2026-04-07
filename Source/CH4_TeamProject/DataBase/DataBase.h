
#pragma once

UENUM(BlueprintType)
enum class EPlayerLifeState : uint8
{
	Alive UMETA(DisplayName = "Alive"), // 에디터에서 표시할 이름
	Downed UMETA(DisplayName = "Downed"),
	Dead UMETA(DisplayName = "Dead") // 4명 모두 다운 상태일 때
};

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	StartStage UMETA(DisplayName = "StartStage"),
	FinalDefense UMETA(DisplayName = "FinalDefense"),
	Clear UMETA(DisplayName = "Clear")
};

FORCEINLINE float DefenceTimer = 300.f;

UENUM(BlueprintType)
enum class EGameResult : uint8
{
	None,
	Win,
	Lose
};

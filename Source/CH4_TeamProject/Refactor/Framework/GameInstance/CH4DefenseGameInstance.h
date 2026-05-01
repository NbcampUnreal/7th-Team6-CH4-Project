/*
 * [Refactor] UCH4DefenseGameInstance
 *
 * 출처: Game/CH4GameInstance.h/cpp 의 UCH4GameInstance
 * 변경:
 *   - 클래스명 UCH4GameInstance -> UCH4DefenseGameInstance (방어전 모드 전용 GameInstance 로 도메인 명시)
 *   - 폴더 위치 Game/ -> Refactor/Framework/GameInstance/
 *   - 사용처 없는 멤버 제거: UpdateTimer (선언만 있고 구현/호출 없음)
 *   - TotalScore/AddToScore/CurrentLevelIndex/RemainingTime 는 BP 의존 가능성 있어 유지
 *   - 본체 동등 (생성자 초기화, AddToScore 본문)
 * 사유: 도메인 명확화 + 폴더 정리 + 데드 멤버 정리
 *
 * 주의: GameInstance 에 게임플레이 상태(점수/타이머/레벨) 저장은 안티패턴.
 *      GameInstance 는 맵 전환을 가로질러 살아남기 때문에 다음 매치에 전판 데이터가
 *      새어나온다. BP 호환성 때문에 본 단계에서는 유지하되, 후속 작업에서
 *      GameState/GameMode 로 이전 검토.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CH4DefenseGameInstance.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4DefenseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCH4DefenseGameInstance();

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddToScore(int32 Amount);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 TotalScore = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
	float RemainingTime = 60.0f;
};

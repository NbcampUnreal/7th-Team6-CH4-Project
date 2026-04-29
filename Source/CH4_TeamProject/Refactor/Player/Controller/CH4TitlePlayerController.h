/*
 * [Refactor] ACH4TitlePlayerController
 *
 * 출처: Title and Lobby/TitlePlayerController.h/cpp 의 ATitlePlayerController
 * 변경:
 *   - 클래스명 ATitlePlayerController -> ACH4TitlePlayerController (CH4 prefix)
 *   - 폴더 위치 "Title and Lobby" -> Refactor/Player/ (공백 폴더 제거 + Player 도메인 통합)
 *   - 부모 ACH4PlayerController 그대로 사용
 *   - 생성자 접근 제한자 public 으로 변경
 *   - 본체 동등 (HUDTitleWidget 생성 및 표시)
 * 사유: CH4 prefix + 폴더 정리
 */
#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"
#include "CH4TitlePlayerController.generated.h"

class UUserWidget;

UCLASS()
class CH4_TEAMPROJECT_API ACH4TitlePlayerController : public ACH4PlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> HUDTitleWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UUserWidget> HUDTitleWidgetInstance;
};

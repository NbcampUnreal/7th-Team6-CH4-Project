/*
 * [Refactor] ACH4HUDBase
 *
 * 출처: UI/CH4HUD.h/cpp 의 ACH4HUD
 * 변경:
 *   - 클래스명 ACH4HUD -> ACH4HUDBase (HUD 도메인 트리의 최상위 추상 클래스)
 *   - 폴더 위치 UI/ -> Refactor/UI/ (Refactor 디렉토리 워크플로)
 *   - 부모 AHUD(엔진)는 Base가 아니므로 자식에 Base 부여 가능
 *   - 본체는 BP가 모든 작업을 담당하므로 C++ 쪽은 진입점만 유지
 *   - 기존의 CrosshairTexture 기반 DrawHUD 구현을 보존하되, BP에서 오버라이드 가능하도록 virtual 유지
 * 사유: BP 베이스로서의 역할 명시 + 폴더 정리
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CH4HUDBase.generated.h"

class UTexture2D;

UCLASS()
class CH4_TEAMPROJECT_API ACH4HUDBase : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TObjectPtr<UTexture2D> CrosshairTexture;
};

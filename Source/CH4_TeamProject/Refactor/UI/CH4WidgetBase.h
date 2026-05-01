/*
 * [Refactor] UCH4WidgetBase
 *
 * 출처: UI/CH4Widget.h 의 UCH4Widget
 * 변경:
 *   - 클래스명 UCH4Widget -> UCH4WidgetBase (Widget 도메인 트리의 최상위 추상 클래스)
 *   - 폴더 위치 UI/ -> Refactor/UI/ (Refactor 디렉토리 워크플로)
 *   - 부모 UUserWidget(엔진)은 Base가 아니므로 자식에 Base 부여 가능
 *   - 본체는 비어있음. BP 위젯들이 본 클래스를 상속하여 모든 UI 로직을 BP에서 구현한다.
 *   - 향후 모든 BP 위젯이 공유해야 하는 공통 C++ 로직(예: 표시/숨김 토글, 입력 차단)
 *     이 생기면 본 클래스에 추가한다.
 * 사유: BP 위젯들의 공통 베이스 역할 명시 + 폴더 정리
 */
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CH4WidgetBase.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4WidgetBase : public UUserWidget
{
	GENERATED_BODY()
};

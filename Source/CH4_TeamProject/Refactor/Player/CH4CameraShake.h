/*
 * [Refactor] UCH4CameraShake
 *
 * 출처: Player/CameraShake/MyCameraShakeBase.h 의 UMyCameraShakeBase
 * 변경:
 *   - 클래스명 UMyCameraShakeBase -> UCH4CameraShake
 *     (My prefix 제거 + CH4 prefix 도입)
 *   - Base suffix 미부여: 부모 UCameraShakeBase(엔진)가 이미 Base 이므로 중복 방지
 *
 *   - 본체는 비어있는 베이스 클래스. 카메라 쉐이크 BP가 본 클래스를 부모로 삼는다.
 * 사유: 템플릿 잔재 prefix 제거 + CH4 통일 + Base 중복 제거
 */
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "CH4CameraShake.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4CameraShake : public UCameraShakeBase
{
	GENERATED_BODY()
};

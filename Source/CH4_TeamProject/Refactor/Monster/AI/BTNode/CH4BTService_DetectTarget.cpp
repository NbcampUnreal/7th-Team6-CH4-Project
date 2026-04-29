#include "CH4BTService_DetectTarget.h"

UCH4BTService_DetectTarget::UCH4BTService_DetectTarget()
{
	NodeName = TEXT("Detect Target");
	Interval = 0.5f;
	RandomDeviation = 0.1f;

	TargetActorKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(UCH4BTService_DetectTarget, TargetActorKey),
		AActor::StaticClass());
}

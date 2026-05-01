#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CH4_TeamProject/Type.h"
#include "CH4ItemData.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4ItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Value = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EEffectType Type = EEffectType::Health;
};

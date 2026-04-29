#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CH4MeleeWeaponAttackNotify.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UCH4MeleeWeaponAttackNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FName FunctionName;
};

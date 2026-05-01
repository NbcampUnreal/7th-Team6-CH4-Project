#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CH4MonsterAnimInstance.generated.h"

class ACH4MonsterBase;
class UCharacterMovementComponent;

UCLASS()
class CH4_TEAMPROJECT_API UCH4MonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Speed = 0.0f;

	UPROPERTY()
	TObjectPtr<ACH4MonsterBase> OwningCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};

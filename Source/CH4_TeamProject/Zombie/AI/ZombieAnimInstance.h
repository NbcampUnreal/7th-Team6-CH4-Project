#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CH4_TeamProject/Zombie/NormalZombie.h"
#include "ZombieAnimInstance.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API UZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Speed = 0.0f;
	
	UPROPERTY()
	TObjectPtr<AZombieBase> OwningCharacter;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CH4_TeamProject/Refactor/Common/CH4Types.h"
#include "CH4PlayerAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class UAnimMontage;

USTRUCT(BlueprintType)
struct FCH4ActionMontageEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> Montage;

	// 재생 전 진행 중인 다른 몽타주를 강제 중단 (Down/Dead/Reload 등 우선순위 높은 액션용).
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	bool bStopOtherMontages = false;
};

UCLASS()
class CH4_TEAMPROJECT_API UCH4PlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayActionMontage(ECH4PlayerActionState NewAction);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetDownState(bool bNewIsDown);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetDeadState(bool bNewIsDead);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void ClearActionState();

	UFUNCTION(BlueprintPure, Category = "Animation")
	ECH4PlayerActionState GetCurrentActionState() const { return CurrentActionState; }

	UFUNCTION()
	void AnimNotify_ClearAction();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TMap<ECH4PlayerActionState, FCH4ActionMontageEntry> ActionMontages;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	bool CanPlayAction(ECH4PlayerActionState NewAction) const;
	bool TryPlayMontage(UAnimMontage* Montage, ECH4PlayerActionState NewAction, bool bStopOtherMontages = false);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UCharacterMovementComponent> Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float MoveSpeed = 350.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDown = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	ECH4PlayerActionState CurrentActionState = ECH4PlayerActionState::None;
};

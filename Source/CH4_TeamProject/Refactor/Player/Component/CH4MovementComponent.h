#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CH4MovementComponent.generated.h"

UCLASS(ClassGroup = (CH4), meta = (BlueprintSpawnableComponent))
class CH4_TEAMPROJECT_API UCH4MovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCH4MovementComponent();

	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	void StartSprint();
	void StopSprint();
	void SetAimWalk(bool bAim);

	UFUNCTION(BlueprintCallable, Category = "Movement|Sprint")
	bool IsSprinting() const { return bWantsToSprint; }

	UFUNCTION(BlueprintCallable, Category = "Movement|Aim")
	bool IsAimWalking() const { return bWantsAimWalk; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed", meta = (ClampMin = "0.0"))
	float SprintSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed", meta = (ClampMin = "0.0"))
	float AimWalkSpeed = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed", meta = (ClampMin = "0.0"))
	float DefaultWalkSpeed = 350.0f;

	// 외부에서 직접 토글 금지. Start*/Stop* 메서드 사용.
	bool bWantsToSprint = false;
	bool bWantsAimWalk = false;
};

// 압축 플래그 슬롯: FLAG_Custom_0 = bWantsToSprint, FLAG_Custom_1 = bWantsAimWalk.
class FSavedMove_CH4 : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	uint8 bSavedWantsToSprint : 1;
	uint8 bSavedWantsAimWalk : 1;

	FSavedMove_CH4();

	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(class ACharacter* Character) override;
};

class FNetworkPredictionData_Client_CH4 : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	FNetworkPredictionData_Client_CH4(const UCharacterMovementComponent& ClientMovement);
	virtual FSavedMovePtr AllocateNewMove() override;
};

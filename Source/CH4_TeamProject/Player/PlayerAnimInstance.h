// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPlayerAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;//애니메이션이 생성되면 호출되는 함수

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;//프레임마다 호출

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	class ACharacter* Owner;//캐릭터 액터

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	class UCharacterMovementComponent* Movement;//무브먼트 컴포넌트

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	FVector Velocity;//속력 저장 변수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float MoveSpeed;//속력에 백터의 2D 구성요소 길이 저장

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	bool isFalling = false;//땅에 닿여 있는지 여부
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ZombieAttackAnimNotify.generated.h"

UENUM(BlueprintType)
enum class EUPHitboxShape : uint8
{
	Sphere,
	Box,
	Capsule
};

UCLASS(DisplayName="Hitbox")
class CH4_TEAMPROJECT_API UZombieAttackAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UZombieAttackAnimNotify();

	virtual FString GetNotifyName_Implementation() const override;
	virtual FLinearColor GetEditorColor() override;

	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hitbox")
	EUPHitboxShape Shape = EUPHitboxShape::Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hitbox")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hitbox", meta=(ClampMin="1.0"))
	float Radius = 50.0f;

private:
	void PerformSphereOverlap(USkeletalMeshComponent* MeshComp) const;
	FVector GetHitboxLocation(USkeletalMeshComponent* MeshComp) const;
	void DrawDebugHitbox(const UWorld* World, const FVector& Center) const;
};

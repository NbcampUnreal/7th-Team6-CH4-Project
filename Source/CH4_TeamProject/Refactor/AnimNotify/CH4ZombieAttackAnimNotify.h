#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CH4_TeamProject/Refactor/Common/CH4Types.h"
#include "CH4ZombieAttackAnimNotify.generated.h"

UCLASS(DisplayName = "Hitbox")
class CH4_TEAMPROJECT_API UCH4ZombieAttackAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UCH4ZombieAttackAnimNotify();

	virtual FString GetNotifyName_Implementation() const override;
	virtual FLinearColor GetEditorColor() override;

	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
	ECH4HitboxShape Shape = ECH4HitboxShape::Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hitbox", meta = (ClampMin = "1.0"))
	float Radius = 50.0f;

private:
	void PerformSphereOverlap(USkeletalMeshComponent* MeshComp) const;
	FVector GetHitboxLocation(USkeletalMeshComponent* MeshComp) const;
	void DrawDebugHitbox(const UWorld* World, const FVector& Center) const;
};

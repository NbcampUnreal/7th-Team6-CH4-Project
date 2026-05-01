#include "CH4ZombieAttackAnimNotify.h"

#include "../Monster/Character/CH4MonsterBase.h"
#include "CH4_TeamProject/Refactor/Player/Character/CH4PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"

static TAutoConsoleVariable<bool> CVarShowHitbox(
	TEXT("Debug.ShowHitbox"),
	false,
	TEXT("Draw hitbox debug shapes"));

UCH4ZombieAttackAnimNotify::UCH4ZombieAttackAnimNotify()
{
	bIsNativeBranchingPoint = true;
}

FString UCH4ZombieAttackAnimNotify::GetNotifyName_Implementation() const
{
	const UEnum* ShapeEnum = StaticEnum<ECH4HitboxShape>();
	const FString ShapeName = ShapeEnum ? ShapeEnum->GetNameStringByValue(static_cast<int64>(Shape)) : TEXT("?");
	return FString::Printf(TEXT("Hitbox: %s R=%.0f"), *ShapeName, Radius);
}

FLinearColor UCH4ZombieAttackAnimNotify::GetEditorColor()
{
	return FLinearColor::Red;
}

void UCH4ZombieAttackAnimNotify::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
	if (!MeshComp) return;

	const UWorld* World = MeshComp->GetWorld();
	const bool bGameWorld = World != nullptr && World->IsGameWorld();
	const FVector Center = GetHitboxLocation(MeshComp);

	if (CVarShowHitbox.GetValueOnGameThread() || !bGameWorld)
	{
		DrawDebugHitbox(World, Center);
	}

	if (!bGameWorld) return;

	switch (Shape)
	{
	case ECH4HitboxShape::Sphere:
		PerformSphereOverlap(MeshComp);
		break;
	default:
		break;
	}
}

void UCH4ZombieAttackAnimNotify::PerformSphereOverlap(USkeletalMeshComponent* MeshComp) const
{
	ACH4MonsterBase* OwnerCharacter = Cast<ACH4MonsterBase>(MeshComp->GetOwner());
	if (!OwnerCharacter) return;
	if (!OwnerCharacter->HasAuthority()) return;

	const FVector Center = GetHitboxLocation(MeshComp);

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;

	const bool bHit = OwnerCharacter->GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params);

	if (!bHit) return;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor) continue;

		const ETeamAttitude::Type Attitude = FGenericTeamId::GetAttitude(OwnerCharacter, HitActor);
		if (Attitude == ETeamAttitude::Friendly) continue;

		ACH4PlayerCharacter* HitCharacter = Cast<ACH4PlayerCharacter>(HitActor);
		if (!HitCharacter) continue;

		UGameplayStatics::ApplyDamage(
			HitCharacter,
			OwnerCharacter->GetAttack(),
			OwnerCharacter->GetController(),
			OwnerCharacter,
			UDamageType::StaticClass());
	}
}

FVector UCH4ZombieAttackAnimNotify::GetHitboxLocation(USkeletalMeshComponent* MeshComp) const
{
	const FVector MeshLocalOffset(-Offset.Y, Offset.X, Offset.Z);
	return MeshComp->GetComponentTransform().TransformPosition(MeshLocalOffset);
}

void UCH4ZombieAttackAnimNotify::DrawDebugHitbox(const UWorld* World, const FVector& Center) const
{
	if (!World) return;

	DrawDebugSphere(World, Center, Radius, 16, FColor::Red, false, 1.0f);
}

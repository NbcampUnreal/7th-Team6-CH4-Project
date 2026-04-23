#include "ZombieAttackAnimNotify.h"
#include "CH4_TeamProject/Player/CH4Character.h"
#include "ZombieBase.h"
#include "GenericTeamAgentInterface.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"

UZombieAttackAnimNotify::UZombieAttackAnimNotify()
{
	// 에디터에서도 Notify 실행되도록 설정
	// 프레임이 밀리지 않게 하기 위해 정확한 프레임에 즉시 실행
	bIsNativeBranchingPoint = true;
}

// 히트박스를 콘솔에서 켜고 끌 수 있는 디버그용 변수
static TAutoConsoleVariable<bool> CVarShowHitbox(
	TEXT("Debug.ShowHitbox"),
	false,
	TEXT("Draw hitbox debug shapes")
);

// 에디터에 표시되는 이름 설정
FString UZombieAttackAnimNotify::GetNotifyName_Implementation() const
{
	// EUPHitboxShape Enum값 저장
	const UEnum* ShapeEnum = StaticEnum<EUPHitboxShape>();
	// Enum값을 문자열로 변환
	const FString ShapeName = ShapeEnum->GetNameStringByValue(static_cast<int64>(Shape));
	// 에디터에 표시되는 이름 출력
	return FString::Printf(TEXT("히트박스: %s R=%.0f"), *ShapeName, Radius);
}

// Notify의 색 표시 설정
FLinearColor UZombieAttackAnimNotify::GetEditorColor()
{
	return FLinearColor::Red;
}

// FBranchingPointNotifyPayload는 이 노티파이를 실행한 캐릭터, 애니메이션, 언제 실행됐나 같은 정보를 가지고 있음
// Branching Point Notify 실행시 Payload의 정보를 사용해 공격 판정을 수행하는 함수
void UZombieAttackAnimNotify::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	// Notify를 실행한 캐릭터의 메쉬 컴포넌트를 가져옴
	USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
	if (MeshComp == nullptr)
	{
		return;
	}

	// 메쉬가 속해있는 월드 정보 저장
	const UWorld* World = MeshComp->GetWorld();
	// 유효한 월드인지, 실제 게임 실행 중인지 확인
	const bool bGameWorld = World != nullptr && World->IsGameWorld();
	// 메쉬 기준으로 히트박스 위치를 계산 후 저장
	const FVector Center = GetHitboxLocation(MeshComp);

	// 히트박스 표시를 켰거나 에디터 모드라면 히트박스 표시
	if (CVarShowHitbox.GetValueOnGameThread() || bGameWorld == false)
	{
		DrawDebugHitbox(World, Center);
	}

	if (bGameWorld == false)
	{
		return;
	}

	// 생성된 히트박스가 Sphere 형태일 때 충돌 확인
	switch (Shape)
	{
	case EUPHitboxShape::Sphere:
		PerformSphereOverlap(MeshComp);
		break;
	default:
		break;
	}
}

// 오버랩시 공격 판정
void UZombieAttackAnimNotify::PerformSphereOverlap(USkeletalMeshComponent* MeshComp) const
{
	// AnimNotify의 주인 정보 저장
	AZombieBase* OwnerCharacter = Cast<AZombieBase>(MeshComp->GetOwner());
	if (OwnerCharacter == nullptr)
	{
		return;
	}
	
	// 서버에서만 데미지 처리
	if (!OwnerCharacter->HasAuthority())
	{
		return;
	}

	// 메쉬 기준으로 계산한 히트박스의 위치를 저장
	const FVector Center = GetHitboxLocation(MeshComp);

	// 히트박스에 충돌한 액터 정보 저장하는 배열
	TArray<FOverlapResult> Overlaps;
	// 충돌 검사 시 사용할 정보
	FCollisionQueryParams Params;

	// 공격 범위 안에 있는 캐릭터의 정보를 Overlaps 배열에 저장
	const bool bHit = OwnerCharacter->GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params
	);
	
	if (bHit == false)
	{
		return;
	}

	// 오버랩 된 액터들 중 유효한 액터만 공격
	for (const FOverlapResult& Overlap : Overlaps)
	{
		// 오버랩 된 액터 정보 저장
		AActor* HitActor = Overlap.GetActor();
		if (HitActor == nullptr)
		{
			continue;
		}

		// Attitude 기반 팀 판정: Friendly면 무시
		const ETeamAttitude::Type Attitude = FGenericTeamId::GetAttitude(OwnerCharacter, HitActor);
		if (Attitude == ETeamAttitude::Friendly)
		{
			continue;
		}

		// HitActor에 저장된 액터의 정보를 ACH4Character로 캐스팅
		ACH4Character* HitCharacter = Cast<ACH4Character>(HitActor);
		if (HitCharacter == nullptr)
		{
			continue;
		}
		
		// 좀비의 공격력을 가져와 캐릭터에게 데미지 전달 (캐릭터의 TakeDamage 자동 호출 됨)
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			OwnerCharacter->GetAttack(),
			OwnerCharacter->GetController(),
			OwnerCharacter,
			UDamageType::StaticClass()
			);
	}
}

// 메쉬 기준으로 계산된 Hitbox의 위치를 월드상의 위치로 변환하는 함수
FVector UZombieAttackAnimNotify::GetHitboxLocation(USkeletalMeshComponent* MeshComp) const
{
	const FVector MeshLocalOffset(-Offset.Y, Offset.X, Offset.Z);
	return MeshComp->GetComponentTransform().TransformPosition(MeshLocalOffset);
}

// 히스박스를 화면에 보이게 표시하는 함수
void UZombieAttackAnimNotify::DrawDebugHitbox(const UWorld* World, const FVector& Center) const
{
	if (World == nullptr)
	{
		return;
	}

	DrawDebugSphere(
		World,
		Center,
		Radius,
		16,
		FColor::Red,
		false,
		1.0f
	);
}
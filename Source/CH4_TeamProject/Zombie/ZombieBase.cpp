#include "ZombieBase.h"

#include "NiagaraFunctionLibrary.h"
#include "Controller/MonsterAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AZombieBase::AZombieBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// AI 컨트롤러를 지정하고 좀비 생성시 자동 빙의 설정
	AIControllerClass = AMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// 좀비끼리 충돌하는 문제를 해결하기 위한 충돌 설정
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	// 이동하는 방향으로 몸을 자동으로 돌림
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 얼마나 빠르게 회전하는지 설정
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	// 회전을 부드럽게 해주는 설정
	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;
}

void AZombieBase::BeginPlay()
{
	Super::BeginPlay();
	// 좀비 이동속도 설정
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

float AZombieBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 실제로 받은 데미지 계산 후 ActualDamage변수에 저장
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 실제로 받은 데미지가 0보다 작으면 0 리턴
	if (ActualDamage < 0)
	{
		return 0.0f;
	}
	
	// 피격 몽타지 플레이
	UAnimInstance* TakeDamageMontageInstance = GetMesh()->GetAnimInstance();
	if (TakeDamageMontageInstance && TakeDamageMontage)
	{
		Multi_PlayTakeDamageMontage(TakeDamageMontage);
	}
	
	// 받은 데미지 계산 후 체력에 적용
	CurrentHP = CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("Hit damage: %f / %f"), CurrentHP, MaxHP);

	// 실제로 받은 데미지 리턴
	return ActualDamage;
}

void AZombieBase::OnDeath()
{
	// 몽타지 플레이
	UAnimInstance* DeathMontageInstance = GetMesh()->GetAnimInstance();
	if (DeathMontageInstance && DeathMontage)
	{
		// 몽타지 재생 및 재생시간 저장
		Multi_PlayDeathMontage(DeathMontage);
		
		float PlayLength = DeathMontage->GetPlayLength();
		
		float Delay = FMath::Max(0.f, PlayLength - 0.3f);
		
		// 타이머를 이용해 몽타지 재생 시간이 끝나기 전에 액터 삭제
		GetWorldTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&AZombieBase::DestroyZombie,
		Delay,
		false
		);
	}
	
	// 지금 움직이고 있는 좀비를 즉시 정지 (속도만 0으로, 입력이 다시 들어오면 움직임)
	GetCharacterMovement()->StopMovementImmediately();
	// 이동 기능을 비활성화 (입력이 들어와도 안 움직임)
	GetCharacterMovement()->DisableMovement();
	
	// 캡슐 컴포넌트 콜리전 지움
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);\
	// 메쉬 콜리전 지움
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AZombieBase::Multi_PlayDeathMontage_Implementation(UAnimMontage* MontageToPlay)
{
	PlayAnimMontage(MontageToPlay);
}

void AZombieBase::Multi_PlayTakeDamageMontage_Implementation(UAnimMontage* MontageToPlay)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	GetWorld(),
	TakeDamageEffect,
	GetActorLocation()
	);
	
	PlayAnimMontage(MontageToPlay);
}

void AZombieBase::Multi_PlayAttackMontage_Implementation(UAnimMontage* MontageToPlay)
{
	PlayAnimMontage(MontageToPlay);
}

void AZombieBase::DestroyZombie()
{
	Destroy();
}

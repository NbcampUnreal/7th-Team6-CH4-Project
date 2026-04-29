#include "CH4MonsterBase.h"

#include "CH4_TeamProject/Refactor/Monster/AI/Controller/CH4ZombieAIController.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "CH4_TeamProject/Refactor/Item/CH4ItemBase.h"

ACH4MonsterBase::ACH4MonsterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = ACH4ZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;
}

void ACH4MonsterBase::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	GetWorld()->GetTimerManager().SetTimer(
		PlaySoundHandle,
		this,
		&ACH4MonsterBase::CurrentSoundPlay,
		20.f,
		true);
}

float ACH4MonsterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage < 0.0f)
	{
		return 0.0f;
	}

	UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInst && TakeDamageMontage)
	{
		Multicast_PlayTakeDamageMontage(TakeDamageMontage);
	}

	CurrentHP = FMath::Clamp(CurrentHP - ActualDamage, 0.0f, MaxHP);

	return ActualDamage;
}

void ACH4MonsterBase::SetZombieStat(float NewHP, float NewDamage, float NewSpeed)
{
	MaxHP = NewHP;
	Damage = NewDamage;
	MoveSpeed = NewSpeed;
}

void ACH4MonsterBase::OnDeath()
{
	UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (AnimInst && DeathMontage)
	{
		Multicast_PlayDeathMontage(DeathMontage);

		const float PlayLength = DeathMontage->GetPlayLength();
		const float Delay = FMath::Max(0.f, PlayLength - 0.3f);

		GetWorldTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&ACH4MonsterBase::DestroyZombie,
			Delay,
			false);
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	const FVector SpawnLocation = GetActorLocation();
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (ItemClass && GetWorld())
	{
		const int32 RollPercent = FMath::RandRange(1, 100);
		if (RollPercent <= 10)
		{
			GetWorld()->SpawnActor<AActor>(ItemClass, SpawnLocation, SpawnRotation, SpawnParams);
		}
	}
}

void ACH4MonsterBase::CurrentSoundPlay()
{
	if (CurrentSound)
	{
		UGameplayStatics::SpawnSoundAttached(CurrentSound, GetRootComponent());
	}
}

void ACH4MonsterBase::Multicast_PlayAttackMontage_Implementation(UAnimMontage* MontageToPlay)
{
	PlayAnimMontage(MontageToPlay);
}

void ACH4MonsterBase::Multicast_PlayTakeDamageMontage_Implementation(UAnimMontage* MontageToPlay)
{
	if (TakeDamageEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			TakeDamageEffect,
			GetActorLocation());
	}

	PlayAnimMontage(MontageToPlay);
}

void ACH4MonsterBase::Multicast_PlayDeathMontage_Implementation(UAnimMontage* MontageToPlay)
{
	PlayAnimMontage(MontageToPlay);
}

void ACH4MonsterBase::DestroyZombie()
{
	Destroy();
}

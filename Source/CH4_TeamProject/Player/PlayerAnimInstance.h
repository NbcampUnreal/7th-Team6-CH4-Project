#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"


//현재 캐릭터가 어떤 행동 상태인지 구분하기 위한 enum
UENUM(BlueprintType)
enum class EPlayerActionState : uint8
{
	None	UMETA(DisplayName = "None"),//0
	Hit		UMETA(DisplayName = "Hit"),//1
	Pickup	UMETA(DisplayName = "Pickup"),//2
	Down	UMETA(DisplayName = "Down"),//3
	Dead	UMETA(DisplayName = "Dead"),//4
	Revive	UMETA(DisplayName = "Revive"),//5

	PistolFire		UMETA(DisplayName = "PistolFire"),
	RifleFire		UMETA(DisplayName = "RifleFire"),
	PistolReload	UMETA(DisplayName = "PistolReload"),
	RifleReload		UMETA(DisplayName = "RifleReload"),
	MeleeAttack     UMETA(DisPlayName = "MeleeAttack"),
	DrinkHealItem   UMETA(DisPlayName = "DrinkHealItem"),
	ShotgunFire		UMETA(DisplayName = "ShotgunFire"),
	ShotgunReload	UMETA(DisplayName = "ShotgunReload")
};

UCLASS()
class CH4_TEAMPROJECT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	//이 AnimInstance를 소유한 캐릭터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	class ACharacter* Owner;


	//캐릭터의 이동 컴포넌트

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	class UCharacterMovementComponent* Movement;


	// 현재 캐릭터의 실제 속도 벡터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	FVector Velocity = FVector::ZeroVector;

	//이동 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float MoveSpeed = 350.0f;


	//캐릭터가 공중에 떠 있는지 여부 점프 / 낙하 애니메이션 전환에 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	bool bIsFalling = false;


	//다운 상태 유지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDown = false;


	//죽은 상태 유지 여부 true면 DeadLoop 또는 사망 상태 유지 포즈로 전환 가능
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;


	//현재 재생 중인 액션 상태 중복 재생 방지, 상태 기반 제한, 충돌 처리에 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EPlayerActionState CurrentActionState = EPlayerActionState::None;

public:
	//피격용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> HitHeadMontage;

	//아이템 줍기용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> PickupMontage;

	//다운 시작용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> DownMontage;

	// 사망 시작용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> DeathMontage;

	//부활 시작용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> ReviveMontage;

	//============총기 관련==========
	//권총 발사용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> PistolFireMontage;

	//라이플 발사용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> RifleFireMontage;

	//권총 장전용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> PistolReloadMontage;

	//라이플 장전용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> RifleReloadMontage;
	
	// 근접무기용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> MeleeAttackMontage;
	
	// 체력 회복 아이템 마시기 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> DrinkHealItemMontage;

	//샷건 발사용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> ShotgunFireMontage;

	//샷건 장전용 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<class UAnimMontage> ShotgunReloadMontage;

protected:
	//특정 행동 검사(상태 마다 안되는것 되는것 판단)
	bool CanPlayAction(EPlayerActionState NewAction) const;

	// ToDo 일단 서버에서 몽타주 재생이없음 
	//몽타주 재생을 담당하는 공통 함수
	bool TryPlayMontage(UAnimMontage* Montage, EPlayerActionState NewAction, bool bStopOtherMontages = false);

public:
	//피격 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayHitHeadMontage();

	//줍기 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayPickupMontage();

	//다운 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayDownMontage();

	//사망 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayDeathMontage();

	//부활 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayReviveMontage();

	//다운 상태 유지 여부 설정
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetDownState(bool bNewIsDown);

	//죽은 상태 유지 여부 설정
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetDeadState(bool bNewIsDead);

	//현재 액션 상태를 None으로 초기화	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void ClearActionState();

	//현재 액션 상태를 반환 디버깅이나 조건 분기용
	UFUNCTION(BlueprintPure, Category = "Animation")
	EPlayerActionState GetCurrentActionState() const { return CurrentActionState; }

	// 권총 발사 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayPistolFireMontage();

	// 라이플 발사 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayRifleFireMontage();

	// 권총 장전 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayPistolReloadMontage();

	// 라이플 장전 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayRifleReloadMontage();
	
	// 근접 공격 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayMeleeAttackMontage();
	
	// 체력 회복 아이템 마시기 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayDrinkHealItemMontage();

	//샷건 발사 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayShotgunFireMontage();

	//샷건 장전 몽타주
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayShotgunReloadMontage();
	
	UFUNCTION()
	void AnimNotify_ClearAction();
};
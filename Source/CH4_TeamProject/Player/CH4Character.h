// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4_TeamProject/Player/PlayerAnimInstance.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4Character.generated.h"
class UEquippableComponent;
class UPlayerAnimInstance;
class ACH4GameState;

UCLASS()
class CH4_TEAMPROJECT_API ACH4Character : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	ACH4Character();

	virtual FGenericTeamId GetGenericTeamId() const override { return static_cast<uint8>(TeamID); }

protected:

	virtual void BeginPlay() override;

public:
	FORCEINLINE ETeamID GetTeamID() const { return TeamID; }

	virtual void Tick(float DeltaTime) override;


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//좀비에게 받을 데메지
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

public:
	//애니메이션 호출 함수
	void PlayHitAnimation();
	void PlayPickupAnimation();
	void PlayDownAnimation();
	void PlayDeathAnimation();
	void PlayReviveAnimation();
	void Multi_PlayAction_Implementation(EPlayerActionState NewState);

	void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayAction(EPlayerActionState NewState);

	UFUNCTION(Server, Reliable)
	void Server_Interact();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	ETeamID TeamID = ETeamID::Player;

	//카메라쪽
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* StimuliSource;


	//함수모음
private:
	void InitializationPlayerMesh();//메쉬 함수

	void InitializationCamera();//카메라 함수

	void InitializationInput();//인풋 함수

	void StartSprint();//뛰기시작

	void StopSprint();//뛰기 멈춤
	
	UFUNCTION()
	void PlayerInputStart();
	
	UFUNCTION()
	void PlayerInputStop();

	void Move(const FInputActionValue& Value);//무브함수

	void Look(const FInputActionValue& Value);//시점 함수

	//프리룩
	void StartFreeLook();
	void StopFreeLook();

	//04-16추가 정조준
	void StartAim();//정조준 시작
	void StopAim();//정조준 멈춤
	void UpdateAimCamera(float DeltaTime);//정조준 했을때 카메라 
	void UpdateRotationMode();//조준상태에 따라 캐릭터 회전방식 통제 함수

	//상호작용
	void Interact();//상호작용
	bool TryReviveNearbyPlayer();//가까이 다운된플레이어
	bool TryPickupNearbyItem();//가까운 아이템


	//입력값(움직임,시점)
private:
	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputMappingContext* DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* SprintAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* FireAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* EquipAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* EquipAction2;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* HealAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* InteractAction;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* ReloadAction;
	
	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* ThrowAction;

	//정조준 액션
	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* AimAction;
	//프리룩
	UPROPERTY(VisibleAnywhere, Category = "Input")
	class UInputAction* FreeLookAction;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	float MaxHP = 100.0f;//최대체력

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stat")
	float CurrentHP = 100.0f;//현재 체력

	//마우스 감도
	UPROPERTY(EditAnywhere)
	float mouseSpeed = 45.0f;

	//플레이어 움직임 속도
	UPROPERTY(EditAnywhere)
	float WalkSpeed = 350.0f;

	//플레이어 뛰기 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 600.0f;

	//달리고있는중인가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;
	//++++++++++++++++++++++++++++++++++++++++++++
	// 조준 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Aim")
	bool bIsAiming = false;

	//프리룩 bool
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|FreeLook")
	bool bIsFreeLook = false;

	// 카메라 기본값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	float DefaultArmLength = 470.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	FVector DefaultSocketOffset = FVector(0.0f, 25.0f, 20.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	float DefaultFOV = 80.0f;

	// 조준 카메라 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	float AimArmLength = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	FVector AimSocketOffset = FVector(0.0f, 55.0f, 15.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	float AimFOV = 65.0f;

	// 보간 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	float AimInterpSpeed = 10.0f;

	// 조준 중 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	float AimWalkSpeed = 220.0f;
	//+++++++++++++++++++++++++++++++++++

	//상호작용 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRadius = 200.0f;

	//장비 장착 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UEquippableComponent> EquippableComponent;

	UFUNCTION(BlueprintCallable)
	void Fires();

	UFUNCTION()
	void OnReload();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Weapon")
	class UWeaponData* PrimaryWeaponData1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Weapon")
	class UWeaponData* PrimaryWeaponData2;
	//장비장착 임렵함수
	void OnEquipInput1();

	void OnEquipInput2();

	UFUNCTION(Server, Reliable)
	void Server_ApplyItemEffect(class AHealItem* HealItem);


	UFUNCTION(NetMulticast, Reliable)
	void HealLog();
	
	void OnApplyItemEffect();
	

	UPROPERTY(Replicated)
	int HealItemCount = 0;

	UPROPERTY(Replicated)
	int GrenadeCount=0;
	
	UPROPERTY()
	ACH4GameState* GamsState;

public:
	UPROPERTY(ReplicatedUsing = OnRep_CombatPose, VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	ECombatPose CurrentCombatPose = ECombatPose::Normal;

	UFUNCTION()
	void OnRep_CombatPose();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateCombatPose();
	void ApplyItemEffect(class UConsumableDataAsset* Data);
	
	UFUNCTION(Server,Reliable)
	void Server_UseHealItem();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UConsumableDataAsset* DefaultHealData;
	
	UFUNCTION(Server, Reliable)
	void Server_ThrowGrenade();	
	
	void ThrowGrenade();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AThorwbleItems> GrenadeClass;
	
	void OnThrowGrenade();
	
	UPROPERTY(Replicated)
	bool bUSingGrenade = false;
	
	void CanUSingGrenade();
	UPROPERTY()
	FTimerHandle GrenadeTimer;
	
	UPROPERTY()
	FTimerHandle ExplosionTimerHandle;
	
	UPROPERTY()
	FTimerHandle PickupMontageTimerHandle;
};

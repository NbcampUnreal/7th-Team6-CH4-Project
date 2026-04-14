// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CH4_TeamProject/Player/PlayerAnimInstance.h"
#include "CH4Character.generated.h"

class UEquippableComponent;
class UPlayerAnimInstance;
class ACH4GameState;
UCLASS()
class CH4_TEAMPROJECT_API ACH4Character : public ACharacter
{
	GENERATED_BODY()

public:

	ACH4Character();

protected:

	virtual void BeginPlay() override;

public:

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

	void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayAction(EPlayerActionState NewState);

	UFUNCTION(Server, Reliable)
	void Server_Interact();
protected:
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

	void Move(const FInputActionValue& Value);//무브함수

	void Look(const FInputActionValue& Value);//시점 함수

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

	//상호작용 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRadius = 200.0f;

	//장비 장착 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UEquippableComponent> EquippableComponent;

	UFUNCTION(BlueprintCallable)
	void Fires();

	UFUNCTION()
	void OnReload();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Weapon")
	class URangedGunDataAsset* PrimaryWeaponData1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Weapon")
	class URangedGunDataAsset* PrimaryWeaponData2;
	//장비장착 임렵함수
	void OnEquipInput1();

	void OnEquipInput2();

	UFUNCTION(Server, Reliable)
	void Server_ApplyItemEffect(class AHealItem* HealItem);

	void ApplyItemEffect(class AHealItem* HealItem);

	UFUNCTION(NetMulticast, Reliable)
	void HealLog();




	void OnApplyItemEffect();

	UPROPERTY()
	TObjectPtr<class AHealItem> Heal;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	int HealItemCount = 0;

	UPROPERTY()
	ACH4GameState* GamsState;

};

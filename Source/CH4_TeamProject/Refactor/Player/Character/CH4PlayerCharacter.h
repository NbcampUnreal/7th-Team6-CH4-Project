// TODO: 생성자의 리소스 경로 하드코딩 제거. 자산 경로를 코드에 박지 말고
// 각 멤버를 UPROPERTY(EditDefaultsOnly) 로 노출하여 BP_Player default 로 자산 지정.
// 대상: SkeletalMesh, AnimInstance, InputMappingContext, IA_* InputAction.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "InputActionValue.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4_TeamProject/Refactor/Common/CH4Types.h"
#include "CH4PlayerCharacter.generated.h"

class UCameraComponent;
class UCH4CameraComponent;
class UCH4SkillComponent;
class UCH4HealthComponent;
class UCH4InteractionComponent;
class UCH4MovementComponent;
class UCH4WeaponData;
class ACH4WeaponBase;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UAIPerceptionStimuliSourceComponent;

UCLASS()
class CH4_TEAMPROJECT_API ACH4PlayerCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	static constexpr int32 MAX_WEAPON_SLOTS = 4;

	ACH4PlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return static_cast<uint8>(TeamID); }

	FORCEINLINE ETeamID GetTeamID() const { return TeamID; }

	UFUNCTION(Server, Reliable)
	void Server_EquipWeaponSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void Fires();

	UFUNCTION()
	void OnReload();

	bool IsReloading() const;

	UFUNCTION()
	void OnRep_CombatPose();

	UFUNCTION(Server, Reliable)
	void Server_UpdateCombatPose();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateCombatPose();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAction(ECH4PlayerActionState NewState);

	void PlayHitAnimation();
	void PlayPickupAnimation();
	void PlayDownAnimation();
	void PlayDeathAnimation();
	void PlayReviveAnimation();

	UFUNCTION(Server, Reliable)
	void Server_FireMontage();

	UFUNCTION()
	void FireMontagePose();

	UFUNCTION(Server, Reliable)
	void Server_ReloadMontage();

	UFUNCTION()
	void ReloadMontage();

	void PlayerDestroy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCH4CameraComponent> CameraLogicComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCH4MovementComponent> CH4MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCH4HealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCH4InteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCH4SkillComponent> SkillComponent;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

	// 정적 자산 참조라 복제 안 함. 슬롯 변동은 CurrentWeapon/CurrentSlotIndex 로만 복제.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Weapon")
	TArray<TObjectPtr<UCH4WeaponData>> PrimaryWeaponDatas;

	// swap 후에도 인스턴스를 보존하여 무기별 탄약 상태를 유지.
	UPROPERTY()
	TArray<TObjectPtr<ACH4WeaponBase>> EquippedWeapons;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat|Weapon")
	TObjectPtr<ACH4WeaponBase> CurrentWeapon;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat|Weapon")
	int32 CurrentSlotIndex = -1;

	// 슬롯 swap 시 무기 간에 양도되는 예비 탄약 풀.
	int32 SharedMaxClip = 0;

	UPROPERTY(ReplicatedUsing = OnRep_CombatPose, VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	ECombatPose CurrentCombatPose = ECombatPose::Normal;

protected:
	virtual void BeginPlay() override;

	// FInputActionValue 시그니처를 요구하는 입력은 별도 핸들러 유지.
	void HandleMoveInput(const FInputActionValue& Value);
	void HandleLookInput(const FInputActionValue& Value);

	// bActive: Toggle 입력은 Started/Completed 양쪽 바인딩, OneShot 입력은 Started 만 바인딩.
	void HandleInputAction(ECH4PlayerInputAction Action, bool bActive);

	UFUNCTION()
	void OnPickupRequestedFromInteraction();

	UFUNCTION()
	void OnReviveRequestedFromInteraction(ACharacter* TargetCharacter);

	UFUNCTION()
	void OnPlayerDownedFromHealth();

	void StopPlayerInput();
	void StartPlayerInput();

	FTimerHandle PickupMontageTimerHandle;
	FTimerHandle PlayerDestroyTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	TMap<ECH4PlayerInputAction, TObjectPtr<UInputAction>> InputActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (ClampMin = "0.0"))
	float MouseSensitivity = 45.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	ETeamID TeamID = ETeamID::Player;

private:
	UCH4WeaponData* GetWeaponDataForSlot(int32 SlotIndex) const;
	ACH4WeaponBase* GetOrSpawnWeaponForSlot(int32 SlotIndex);

	UPROPERTY()
	TObjectPtr<class ACH4DefenseGameState> GameState;
};

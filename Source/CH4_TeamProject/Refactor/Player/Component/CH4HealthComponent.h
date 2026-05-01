#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CH4HealthComponent.generated.h"

class ACharacter;
class AController;
struct FDamageEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCH4OnPlayerDowned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCH4OnPlayerDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCH4OnHealthChanged, float, NewHP);

UCLASS(ClassGroup = (CH4), meta = (BlueprintSpawnableComponent))
class CH4_TEAMPROJECT_API UCH4HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCH4HealthComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Character::TakeDamage 가 위임. Server 권한에서만 HP 갱신/다운 처리.
	float ApplyDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Server 권한에서만 호출.
	void Heal(float Amount);

	// 소생. bIsDowned 리셋 + 이동 재활성화 + HP 부분 회복 (Server 권한에서만 호출).
	void Revive(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Player Stat")
	bool IsDowned() const { return bIsDowned; }

	UFUNCTION(BlueprintCallable, Category = "Player Stat")
	float GetMaxHP() const { return MaxHP; }

	UFUNCTION(BlueprintCallable, Category = "Player Stat")
	float GetCurrentHP() const { return CurrentHP; }

	UFUNCTION()
	void OnRep_IsDowned();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat", Replicated, meta = (ClampMin = "0.0"))
	float MaxHP = 150.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stat", Replicated)
	float CurrentHP = 150.0f;

	UPROPERTY(ReplicatedUsing = OnRep_IsDowned, VisibleAnywhere, BlueprintReadOnly, Category = "Player Stat")
	bool bIsDowned = false;

	UPROPERTY(BlueprintAssignable, Category = "Player Stat")
	FCH4OnPlayerDowned OnPlayerDowned;

	UPROPERTY(BlueprintAssignable, Category = "Player Stat")
	FCH4OnPlayerDied OnPlayerDied;

	UPROPERTY(BlueprintAssignable, Category = "Player Stat")
	FCH4OnHealthChanged OnHealthChanged;

protected:
	void HandleDowned();
	void HandleDeath();

private:
	ACharacter* GetOwnerCharacter() const;
};

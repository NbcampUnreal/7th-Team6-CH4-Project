#pragma once

#include "CoreMinimal.h"
#include "CH4WeaponBase.h"
#include "CH4RangedWeapon.generated.h"

class UAIPerceptionStimuliSourceComponent;

UCLASS()
class CH4_TEAMPROJECT_API ACH4RangedWeapon : public ACH4WeaponBase
{
	GENERATED_BODY()

public:
	ACH4RangedWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Attack_Implementation_Internal() override;
	virtual void Reload_Implementation_Internal() override;
	virtual void AddMaxClip(int32 AmmoItem) override;

	UFUNCTION()
	virtual void Attack_Implementation();
	virtual void Attack();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyDamageToTarget(AActor* TargetActor);

	void ProcessDamage(AActor* TargetActor);

	UFUNCTION()
	void OnRep_FireReady();

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(ReplicatedUsing = OnRep_FireReady)
	bool bIsCoolingDown = false;

protected:
	virtual void BeginPlay() override;

	virtual int32 GetMaxAmmo() const override;
	UFUNCTION(BlueprintPure)
	virtual int32 GetMaxClip() const override;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayEffects(FVector TraceStart, FVector TraceEnd, bool bHit);

	void TraceShoot();
	void ProcessReload();
	void ResetCoolTime();

	FTimerHandle TimerHandle_FireDelay;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Equippable.h"
#include "RangedWeapons.generated.h"

class UWeaponData;
UCLASS()
class CH4_TEAMPROJECT_API ARangedWeapons : public AEquippable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UPROPERTY(BlueprintReadWrite, EditAnywhere,Category="DataComponent")
	TObjectPtr<class UEquippableComponent> WeaponComponent;
	
	ARangedWeapons();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyDamageToTarget(AActor* TargetActor);
	
	void ProcessDamage(AActor* TargetActor);
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	virtual void Attack_Implementation() ;
	
	virtual void Attack() ;
	virtual void Attack_Implementation_Internal() override;
	
	

	virtual void Reload_Implementation_Internal() override;
	
	UPROPERTY(ReplicatedUsing = OnRep_FireReady)
	bool bIsCoolingDown= false;
	
	virtual void AddMaxClip(int32 AmmoItem )override;
	

	
protected:	
	virtual void BeginPlay() override;
	
	
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayEffects(FVector TraceStart, FVector TraceEnd, bool bHit);
	
	void TraceShoot();

	virtual int32 GetMaxAmmo()const override;
	virtual int32 GetMaxClip() const override;
	
	void ProcessReload();
	
	
	FTimerHandle TimerHandle_FireDelay;
	UFUNCTION()
	void OnRep_FireReady(); 
	
	void ResetCoolTime();
};

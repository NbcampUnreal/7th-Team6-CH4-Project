// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Equippable.h"
#include "RangedWeapons.generated.h"

class URangedGunDataAsset;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	int32 GetCurrentAmmo ()const
	{
		return CurrentAmmo; 
	} 
	
	int32 GetMaxAmmo () const;
	
	int32 GetMaxClip()const
	{
		return MaxClip;
	}
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(Replicated)
	int32 CurrentAmmo;

	UPROPERTY(Replicated)
	int32 MaxClip = 300;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Fire();
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayEffects();

	void TraceShoot();
	
	UFUNCTION(BlueprintCallable)
	virtual void Fire();
	
	UPROPERTY(EditAnywhere,Category="DataAsset")
	URangedGunDataAsset* GunDataAsset;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ReLoad();
	
	void ProcessReload();
};

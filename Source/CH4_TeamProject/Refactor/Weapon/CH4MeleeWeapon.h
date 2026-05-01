#pragma once

#include "CoreMinimal.h"
#include "CH4WeaponBase.h"
#include "CH4MeleeWeapon.generated.h"

class UAnimMontage;
class UCH4MeleeWeaponData;
class USphereComponent;

UCLASS()
class CH4_TEAMPROJECT_API ACH4MeleeWeapon : public ACH4WeaponBase
{
	GENERATED_BODY()

public:
	ACH4MeleeWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Attack_Implementation_Internal() override;
	virtual void Attack();

	UAnimMontage* GetAttackMontage() const { return MeleeAttackMontage; }

	UFUNCTION(BlueprintCallable)
	void EnableMeleeCollision();

	UFUNCTION(BlueprintCallable)
	void DisableMeleeCollision();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttackMontage();

	UFUNCTION(Server, Reliable)
	void Server_EnableMeleeCollision();

	UFUNCTION(Server, Reliable)
	void Server_DisableMeleeCollision();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<UCH4MeleeWeaponData> MeleeData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", Replicated)
	TObjectPtr<UAnimMontage> MeleeAttackMontage;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMeleeOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USphereComponent> MeleeDetector;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActors;
};

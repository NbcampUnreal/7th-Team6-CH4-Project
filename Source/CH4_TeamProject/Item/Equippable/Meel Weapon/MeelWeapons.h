// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Item/Equippable/Equippable.h"
#include "MeelWeapons.generated.h"


class UMeleeWeaponData;
class USphereComponent; // 스피어 콜리전을 위해 전방 선언

UCLASS()
class CH4_TEAMPROJECT_API AMeleeWeapons : public AEquippable
{
	GENERATED_BODY()

public:
	
	AMeleeWeapons();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USphereComponent> MeleeDetector;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActors;
	
	UFUNCTION()
	void OnMeleeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable)
	void EnableMeleeCollision();

	UFUNCTION(BlueprintCallable)
	void DisableMeleeCollision();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TObjectPtr<UMeleeWeaponData> MeelData;
	
	virtual void Attack_Implementation_Internal() override;
	virtual void Attack();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation",Replicated)
	TObjectPtr<UAnimMontage> MeleeAttackMontage;
	
	UAnimMontage* GetAttackMontage() const { return MeleeAttackMontage; }
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttackMontage(); 
	
	UFUNCTION(Server, Reliable)
	void Server_EnableMeleeCollision();

	UFUNCTION(Server, Reliable)
	void Server_DisableMeleeCollision();
	
};
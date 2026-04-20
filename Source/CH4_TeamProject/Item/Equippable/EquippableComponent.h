// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquippableComponent.generated.h"

class AEquippable;
class URangedGunDataAsset;
class UMeleeWeaponData;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CH4_TEAMPROJECT_API UEquippableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEquippableComponent();
	// 데이터 에셋몇개든 관리할함수
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<int32, UPrimaryDataAsset*> AllWeaponDataAsset;


	//장비 장착함수
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EquipWeapon(class UWeaponData* WeaponClass);

	//착용중인 무기
	UPROPERTY(Replicated)
	TObjectPtr<AEquippable> CurrentWeapon;

	UPROPERTY(Replicated)
	TObjectPtr<UWeaponData> AllCurrentWeapon;


	// 컴포넌트에서 발사
	void Fire();

	void Reload();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool UsingWeapon = false;

	//기억할 총알갯수
	UPROPERTY()
	TMap<TSubclassOf<AEquippable>, int32> WeaponAmmoMemory;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FTransform GetLeftHandSocketTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool HasCurrentWeapon() const;

private:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"

/**
 * 
 */
class AEquippable;
class UStaticMesh;

UCLASS()
class CH4_TEAMPROJECT_API UWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Common")
	TSubclassOf<AEquippable> WeaponClass;

	UPROPERTY(EditAnywhere, Category="Weapon|visual")
	TObjectPtr<UStaticMesh> StaticMesh;
	
	UPROPERTY(EditAnywhere, Category="Common")
	int32 Damage = 0;
	
	virtual	FPrimaryAssetId GetPrimaryAssetId() const override;
	
	virtual int32 GetMaxAmmo() const {  UE_LOG(LogTemp, Warning, TEXT("부모 GetMaxAmmo 호출됨: %d"), 0) ;return 0; }
	
	virtual float GetRangedLength() const { return 0.0f; }
	
	virtual float GetFireRate() const { return 0.0f; }
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	class USoundBase* FireSound; 

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraSystem* MuzzleFlash; 



	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	ECombatPose CombatPose = ECombatPose::Rifle;

	virtual ECombatPose GetGunDataAsset() const 
	{
		return CombatPose;
	}

	virtual void SetGunDataAsset(ECombatPose NewCombatPose) 
	{
		CombatPose = NewCombatPose;
	}
};

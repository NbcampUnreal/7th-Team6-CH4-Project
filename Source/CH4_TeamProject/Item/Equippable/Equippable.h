// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseItem.h"
#include "../../DataBase/DataBase.h"
#include "Equippable.generated.h"


UCLASS()
class CH4_TEAMPROJECT_API AEquippable : public ABaseItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEquippable();

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data",ReplicatedUsing = OnRep_WeaponData)
	TObjectPtr<class UWeaponData> WeaponData;
	
	UFUNCTION()
	void OnRep_WeaponData();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 모든 총이나 무기의 공격함수
	UFUNCTION(Server,Reliable)
	void Server_Attack();
	
	virtual void Attack_Implementation_Internal();
	
	
	// 총의 재장선 함수
	UFUNCTION(Server,Reliable)
	void Server_Reload();
	
	virtual void Reload_Implementation_Internal();
	
	virtual void AddMaxClip(int32 AmmoItem ){};
	
	// 무기의 메쉬들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	// 최대 총알갯수
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 MaxClip = 0;

	// 현재 갯수GetLifetimeReplicatedProps
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentAmmo = 0;

	// 장전된 갯수는 상속받은 총의 데이터 에셋에서 가져다 사용할것
	virtual int32 GetMaxClip() const { return 0; }
	
	virtual int32 GetMaxAmmo () const {return 0;}
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	UPROPERTY(EditAnywhere,Category="DataAsset",Replicated)
	class UWeaponData* DataAsset;
	
	// const int32 GetCurrentAmmo() { return CurrentAmmo; }
	virtual void SetCurrentAmmo();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Animation")
	ECombatPose CombatPose = ECombatPose::Rifle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> DefaultSceneRoot;	
	
	UPROPERTY(ReplicatedUsing = OnRep_WeaponMeshCollision)
	bool bCollisionDisabled = false;
	
	UFUNCTION()
	void OnRep_WeaponMeshCollision();

};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4WeaponBase.generated.h"

class ACharacter;
class UCameraShakeBase;
class UCH4WeaponData;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class CH4_TEAMPROJECT_API ACH4WeaponBase : public AActor
{
	GENERATED_BODY()

public:
	static const FName HandSocketName;

	ACH4WeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Equip")
	void Equip(ACharacter* OwnerCharacter);

	// Unequip 은 액터를 파괴하지 않고 숨김 처리만 한다. 인벤토리 슬롯에서 탄약/Clip 상태를 보존해야 하기 때문.
	UFUNCTION(BlueprintCallable, Category = "Weapon|Equip")
	void Unequip();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Effects")
	void PlayWeaponShake();

	UFUNCTION()
	void OnRep_WeaponData();

	virtual int32 GetMaxClip() const { return 0; }
	virtual int32 GetMaxAmmo() const { return 0; }
	virtual void SetCurrentAmmo();
	virtual void AddMaxClip(int32 AmmoItem) {}

	UFUNCTION(Server, Reliable)
	void Server_Attack();

	virtual void Attack_Implementation_Internal();

	UFUNCTION(Server, Reliable)
	void Server_Reload();

	virtual void Reload_Implementation_Internal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data", ReplicatedUsing = OnRep_WeaponData)
	TObjectPtr<UCH4WeaponData> WeaponDataAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 MaxClip = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	ECombatPose WeaponPoseType = ECombatPose::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
	TSubclassOf<UCameraShakeBase> CamShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects", meta = (ClampMin = "0.0"))
	float InnerRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects", meta = (ClampMin = "0.0"))
	float OuterRadius = 2500.0f;
};

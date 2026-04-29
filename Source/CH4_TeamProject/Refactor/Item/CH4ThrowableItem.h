#pragma once

#include "CoreMinimal.h"
#include "CH4ItemBase.h"
#include "CH4ThrowableItem.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;
class USoundBase;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class CH4_TEAMPROJECT_API ACH4ThrowableItem : public ACH4ItemBase
{
	GENERATED_BODY()

public:
	ACH4ThrowableItem();

	void Explode();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayExplosionEffects();

	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UPROPERTY()
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (ClampMin = "0.0"))
	float ExplosionDamage = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (ClampMin = "0.0"))
	float ExplosionRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	TObjectPtr<USoundBase> ExplosionSound;
};

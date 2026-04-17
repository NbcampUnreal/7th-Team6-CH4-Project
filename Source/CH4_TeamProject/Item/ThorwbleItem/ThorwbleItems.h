// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Item/BaseItem.h"
#include "GameFramework/Actor.h"
#include "ThorwbleItems.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AThorwbleItems : public ABaseItem
{
	GENERATED_BODY()
	
public:	

	AThorwbleItems();
	
protected:
	
	virtual void BeginPlay() override;
	
public:	

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionDamage = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionRadius = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	class UParticleSystem* ExplosionEffect;
	
	void Explode();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayExplosionEffects();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	


};

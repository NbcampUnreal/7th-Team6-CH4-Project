#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CH4SkillComponent.generated.h"

class ACharacter;
class ACH4ThrowableItem;
class UCH4ItemData;
class UCH4HealthComponent;

UCLASS(ClassGroup = (CH4), meta = (BlueprintSpawnableComponent))
class CH4_TEAMPROJECT_API UCH4SkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCH4SkillComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnApplyItemEffect();
	void OnThrowGrenade();

	// InteractionComponent 가 줍기 시 호출.
	void ApplyItemEffect(UCH4ItemData* Data);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TObjectPtr<UCH4ItemData> DefaultHealData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<ACH4ThrowableItem> GrenadeClass;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skill")
	int32 HealItemCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skill")
	int32 GrenadeCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skill")
	bool bIsUsingGrenade = false;

protected:
	UFUNCTION(Server, Reliable)
	void Server_UseHealItem();

	UFUNCTION(Server, Reliable)
	void Server_ThrowGrenade();

	void ThrowGrenade();

	UFUNCTION()
	void CanUseGrenade();

private:
	ACharacter* GetOwnerCharacter() const;
	UCH4HealthComponent* FindHealthComponent() const;

	UPROPERTY()
	FTimerHandle GrenadeTimer;

	UPROPERTY()
	FTimerHandle ExplosionTimerHandle;
};

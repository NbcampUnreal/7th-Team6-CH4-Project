#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4MonsterBase.generated.h"

class ACH4ItemBase;
class AGearItem;
class UAnimMontage;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class CH4_TEAMPROJECT_API ACH4MonsterBase : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACH4MonsterBase();

	virtual void BeginPlay() override;
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return static_cast<uint8>(TeamID); }

	virtual void SetZombieStat(float NewHP, float NewDamage, float NewSpeed);
	virtual void OnDeath();

	void CurrentSoundPlay();

	FORCEINLINE ETeamID GetTeamID() const { return TeamID; }

	UFUNCTION(BlueprintCallable, Category = "Stat")
	float GetAttack() const { return Damage; }

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttackMontage(UAnimMontage* MontageToPlay);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayTakeDamageMontage(UAnimMontage* MontageToPlay);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDeathMontage(UAnimMontage* MontageToPlay);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MoveSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> TakeDamageMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<ACH4ItemBase> ItemClass;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> TakeDamageEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> CurrentSound;

	FTimerHandle DestroyTimerHandle;
	FTimerHandle PlaySoundHandle;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	ETeamID TeamID = ETeamID::Monster;

private:
	void DestroyZombie();
};

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "NiagaraSystem.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "GameFramework/Character.h"
#include "Iris/ReplicationSystem/ReplicationSystemTypes.h"
#include "ZombieBase.generated.h"

class AGearItem;
class UAnimMontage;

UCLASS()
class CH4_TEAMPROJECT_API AZombieBase : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AZombieBase();
	
	virtual void BeginPlay() override;
	void CurrentSoundPlay();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDeath();
	
	virtual FGenericTeamId GetGenericTeamId() const override { return static_cast<uint8>(TeamID); }

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayAttackMontage(UAnimMontage* MontageToPlay);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayTakeDamageMontage(UAnimMontage* MontageToPlay);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayDeathMontage(UAnimMontage* MontageToPlay);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MoveSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Damage;
	
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TakeDamageEffect;
	
	UPROPERTY(EditAnywhere)
	USoundBase* CurrentSound;
	
	virtual void SetZombieStat(float NewHP, float NewDamage, float NewSpeed);
	
	float GetAttack() { return Damage; };
	
	FORCEINLINE ETeamID GetTeamID() const  { return TeamID; }
	
	FTimerHandle DestroyTimerHandle;
	FTimerHandle PlaySoundHandle;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Team")
	ETeamID TeamID = ETeamID::Monster;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* TakeDamageMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AGearItem> ItemClass;
private:
	void DestroyZombie();
};
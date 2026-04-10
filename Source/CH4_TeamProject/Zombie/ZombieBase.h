#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "TeamID.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

class UAnimMontage;

UCLASS()
class CH4_TEAMPROJECT_API AZombieBase : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AZombieBase();
	
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayAttackMontage(UAnimMontage* MontageToPlay);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayTakeDamageMontage(UAnimMontage* MontageToPlay);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayDeathMontage(UAnimMontage* MontageToPlay);
	
	//virtual FGenericTeamId GetGenericTeamId() const override { return static_cast<uint8>(TeamID); }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MoveSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Damage;
	
	float GetAttack() { return Damage; };
	
	//FORCEINLINE ETeamID GetTeamID() const  { return TeamID; }
	
	FTimerHandle DestroyTimerHandle;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* TakeDamageMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* DeathMontage;
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	//ETeamID TeamID = ETeamID::NoTeam;
	
private:
	void DestroyZombie();
};
#include "CH4SkillComponent.h"

#include "CH4HealthComponent.h"
#include "CH4_TeamProject/Refactor/Framework/GameState/CH4DefenseGameState.h"
#include "CH4_TeamProject/Refactor/Item/CH4ThrowableItem.h"
#include "CH4_TeamProject/Refactor/Item/Data/CH4ItemData.h"
#include "CH4_TeamProject/Refactor/Weapon/CH4WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

UCH4SkillComponent::UCH4SkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UCH4SkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCH4SkillComponent, HealItemCount);
	DOREPLIFETIME(UCH4SkillComponent, GrenadeCount);
	DOREPLIFETIME(UCH4SkillComponent, bIsUsingGrenade);
}

void UCH4SkillComponent::OnApplyItemEffect()
{
	if (HealItemCount > 0)
	{
		Server_UseHealItem();
	}
}

void UCH4SkillComponent::OnThrowGrenade()
{
	ThrowGrenade();
}

void UCH4SkillComponent::ApplyItemEffect(UCH4ItemData* Data)
{
	if (!Data) return;

	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return;

	switch (Data->Type)
	{
	case EEffectType::Ammo:
		{
			TArray<AActor*> AttachedActors;
			OwnerChar->GetAttachedActors(AttachedActors);

			for (AActor* Actor : AttachedActors)
			{
				if (ACH4WeaponBase* FoundWeapon = Cast<ACH4WeaponBase>(Actor))
				{
					FoundWeapon->AddMaxClip(Data->Value);
					return;
				}
			}
		}
		break;

	case EEffectType::Health:
		{
			HealItemCount++;
		}
		break;

	case EEffectType::Gear:
		{
			if (UWorld* World = GetWorld())
			{
				if (ACH4DefenseGameState* GS = World->GetGameState<ACH4DefenseGameState>())
				{
					GS->AddGearPartsCount();
				}
			}
		}
		break;

	case EEffectType::Grenade:
		{
			GrenadeCount++;
		}
		break;

	default:
		break;
	}
}

void UCH4SkillComponent::Server_UseHealItem_Implementation()
{
	const ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->HasAuthority()) return;

	UCH4HealthComponent* Health = FindHealthComponent();
	if (!Health) return;
	if (Health->IsDowned()) return;

	if (DefaultHealData && HealItemCount > 0)
	{
		Health->Heal(DefaultHealData->Value);
		HealItemCount--;
	}
}

void UCH4SkillComponent::Server_ThrowGrenade_Implementation()
{
	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->HasAuthority()) return;

	if (bIsUsingGrenade) return;
	if (GrenadeCount <= 0) return;
	if (!GrenadeClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector SpawnLocation = OwnerChar->GetMesh()
		? OwnerChar->GetMesh()->GetSocketLocation(TEXT("Weapon_r"))
		: OwnerChar->GetActorLocation();
	const FRotator SpawnRotation = OwnerChar->GetControlRotation();

	if (ACH4ThrowableItem* Grenade = World->SpawnActor<ACH4ThrowableItem>(GrenadeClass, SpawnLocation, SpawnRotation))
	{
		GrenadeCount--;
		Grenade->SetOwner(OwnerChar);
		Grenade->SetInstigator(OwnerChar);

		if (Grenade->ProjectileMovement)
		{
			const FVector ThrowVelocity = SpawnRotation.Vector() * 1500.0f;
			Grenade->ProjectileMovement->Velocity = ThrowVelocity;
		}

		World->GetTimerManager().SetTimer(ExplosionTimerHandle, Grenade, &ACH4ThrowableItem::Explode, 1.5f, false);
		bIsUsingGrenade = true;
	}

	World->GetTimerManager().SetTimer(GrenadeTimer, this, &UCH4SkillComponent::CanUseGrenade, 5.0f, false);
}

void UCH4SkillComponent::ThrowGrenade()
{
	Server_ThrowGrenade();
}

void UCH4SkillComponent::CanUseGrenade()
{
	bIsUsingGrenade = false;
}

ACharacter* UCH4SkillComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

UCH4HealthComponent* UCH4SkillComponent::FindHealthComponent() const
{
	const AActor* Owner = GetOwner();
	return Owner ? Owner->FindComponentByClass<UCH4HealthComponent>() : nullptr;
}

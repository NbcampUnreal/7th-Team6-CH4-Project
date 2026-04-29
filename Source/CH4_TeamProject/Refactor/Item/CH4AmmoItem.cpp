#include "CH4AmmoItem.h"

#include "../Weapon/CH4RangedWeapon.h"
#include "../Weapon/CH4WeaponBase.h"
#include "Data/CH4ItemData.h"
#include "GameFramework/Character.h"

ACH4AmmoItem::ACH4AmmoItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACH4AmmoItem::BeginPlay()
{
	Super::BeginPlay();

	if (ItemData)
	{
		ItemData->Type = EEffectType::Ammo;
	}
}

void ACH4AmmoItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (!Player || !ItemData) return;

	TArray<AActor*> AttachedActors;
	Player->GetAttachedActors(AttachedActors);

	for (AActor* Actor : AttachedActors)
	{
		if (ACH4WeaponBase* FoundWeapon = Cast<ACH4RangedWeapon>(Actor))
		{
			FoundWeapon->AddMaxClip(ItemData->Value);
			Destroy();
			return;
		}
	}
}

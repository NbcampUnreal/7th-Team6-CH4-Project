// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoItem.h"

#include "ConsumableDataAsset.h"
#include "../Equippable/Ranged Weapon/RangedWeapons.h"
#include "CH4_TeamProject/Item/Equippable/WeaponData.h"
#include "CH4_TeamProject/Player/CH4Character.h"

// Sets default values
AAmmoItem::AAmmoItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAmmoItem::BeginPlay()
{
	Super::BeginPlay();
	DataAsset->Type = EEffectType::Ammo;
}

// Called every frame
void AAmmoItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmoItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	ACH4Character* player = Cast<ACH4Character>(OtherActor);
	if (player)
	{
		TArray<AActor*> AttachedActors;
		player->GetAttachedActors(AttachedActors);

		for (AActor* Actor : AttachedActors)
		{
			AEquippable* FoundWeapon = Cast<ARangedWeapons>(Actor);
			if (FoundWeapon)
			{
				FoundWeapon->AddMaxClip(DataAsset->Value);
				Destroy();
				return; 
			}
		}
        
	}
}
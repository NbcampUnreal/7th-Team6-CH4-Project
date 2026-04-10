// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoItem.h"
#include "../Equippable/Ranged Weapon/RangedWeapons.h"
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
		// 2. ⭐️ 플레이어는 총을 모르니까, 플레이어한테 붙어있는 "모든 자식 액터"를 뒤져서 무기를 찾아!
		TArray<AActor*> AttachedActors;
		player->GetAttachedActors(AttachedActors);

		for (AActor* Actor : AttachedActors)
		{
			// 3. 그중에서 "무기(ARangedWeapons)" 클래스인 놈이 있는지 확인
			ARangedWeapons* FoundWeapon = Cast<ARangedWeapons>(Actor);
			if (FoundWeapon)
			{
				// 4. 찾았다! 플레이어는 몰라도 아이템은 이 무기를 바로 건드린다.
				FoundWeapon->AddMaxClip(AmmoItem);
                
				UE_LOG(LogTemp, Warning, TEXT("야후: 플레이어는 몰랐지만 무기를 찾아내서 충전 완료!%d:총알"),FoundWeapon->GetMaxClip());
				Destroy();
				return; // 찾았으니 끝!
			}
		}
        
		UE_LOG(LogTemp, Error, TEXT("야후: 플레이어 주변에 무기가 안 보여!"));
	}
}
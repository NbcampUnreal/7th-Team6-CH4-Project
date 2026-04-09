// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Ranged Weapon/RangedWeapons.h"


class URangedGunDataAsset;
// Sets default values for this component's properties
UEquippableComponent::UEquippableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEquippableComponent::Fire()
{
	CurrentWeapon->Fire();
}

void UEquippableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquippableComponent, CurrentWeapon);
}

void UEquippableComponent::EquipWeapon_Implementation(TSubclassOf<class ARangedWeapons> WeaponClass)
{
	if (!GetOwner()->HasAuthority() || !WeaponClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EquipWeapon Called!"));
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("EquipWeapon Called!"));

	UWorld* World = GetWorld();

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}

	FActorSpawnParameters SpawnActor;
	SpawnActor.Owner = GetOwner();
	SpawnActor.Instigator = Cast<APawn>(GetOwner());
	SpawnActor.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = World->SpawnActor<ARangedWeapons>
	(WeaponClass,
	 GetOwner()->GetActorLocation(),
	 GetOwner()->GetActorRotation(),
	 SpawnActor);
	if (CurrentWeapon)
	{
		ACharacter* MyCharacter = Cast<ACharacter>(GetOwner());
		if (MyCharacter)
		{
			CurrentWeapon->AttachToComponent(MyCharacter->GetMesh(),
			                                 FAttachmentTransformRules::SnapToTargetIncludingScale,
			                                 FName("Weapon_r")
			);
		}
		//UsingWeapon = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("스폰실패"));
	}
	
}

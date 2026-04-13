// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Ranged Weapon/RangedWeaponDataAsset.h"
#include "Ranged Weapon/RangedWeapons.h"


class URangedGunDataAsset;
// Sets default values for this component's properties
UEquippableComponent::UEquippableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UEquippableComponent::Fire()
{
	if (CurrentWeapon == nullptr)
	{
		return;
	}
	CurrentWeapon->Fire();
}

void UEquippableComponent::Reload()
{
	if (CurrentWeapon == nullptr)
	{
		return;
	}
	CurrentWeapon->Server_ReLoad();
}


void UEquippableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquippableComponent, CurrentWeapon);
}

void UEquippableComponent::EquipWeapon_Implementation(URangedGunDataAsset* WeaponData)
{
	if (!GetOwner()->HasAuthority() || !WeaponData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EquipWeapon Called!"));
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("EquipWeapon Called!"));

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (CurrentWeapon)
	{	
		WeaponAmmoMemory.Add(CurrentWeapon->GetClass(), CurrentWeapon->CurrentAmmo);
		CurrentWeapon->Destroy();
	}
	
	FActorSpawnParameters SpawnActor;
	SpawnActor.Owner = GetOwner();
	SpawnActor.Instigator = Cast<APawn>(GetOwner());
	SpawnActor.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = World->SpawnActor<ARangedWeapons>
	(WeaponData->WeaponClass,
	 GetOwner()->GetActorLocation(),
	 GetOwner()->GetActorRotation(),
	 SpawnActor);
	
	if (CurrentWeapon)
	{
		ACharacter* MyCharacter = Cast<ACharacter>(GetOwner());
		if (MyCharacter)
		{
			CurrentWeapon->AttachToComponent
			(MyCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			FName("Weapon_r")
			);
		}
		//UsingWeapon = true;
	}
	if (CurrentWeapon)
	{
		if (WeaponAmmoMemory.Contains(WeaponData->WeaponClass))
		{
			CurrentWeapon->CurrentAmmo = (WeaponAmmoMemory[WeaponData->WeaponClass]);
		}
		else
		{
			CurrentWeapon->SetCurrentAmmo();
		}
	}
}
	

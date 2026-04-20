// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Ranged Weapon/RangedWeaponDataAsset.h"
#include "Ranged Weapon/RangedWeapons.h"
#include "Components/StaticMeshComponent.h"
#include "Equippable.h"


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
	CurrentWeapon->Server_Attack();
}

void UEquippableComponent::Reload()
{
	if (CurrentWeapon == nullptr)
	{
		return;
	}
	
	CurrentWeapon->Server_Reload();
}


void UEquippableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquippableComponent, CurrentWeapon);
	DOREPLIFETIME(UEquippableComponent, UsingWeapon); 
}

void UEquippableComponent::EquipWeapon_Implementation(UWeaponData* NewWeaponData)
{
	if (!GetOwner()->HasAuthority() || !NewWeaponData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EquipWeapon Called!"));
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("함수 호출 성공"));

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("월드 존재함"));
	if (CurrentWeapon)
	{	
		WeaponAmmoMemory.Add(CurrentWeapon->GetClass(), CurrentWeapon->CurrentAmmo);
		CurrentWeapon->Destroy();
	}
	
	FActorSpawnParameters SpawnActor;
	SpawnActor.Owner = GetOwner();
	SpawnActor.Instigator = Cast<APawn>(GetOwner());
	SpawnActor.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = World->SpawnActor<AEquippable>
	(NewWeaponData->WeaponClass,
	 GetOwner()->GetActorLocation(),
	 GetOwner()->GetActorRotation(),
	 SpawnActor);

	if (CurrentWeapon)
	{
		CurrentWeapon->WeaponData->SetGunDataAsset(NewWeaponData->GetGunDataAsset());
	}
	
	CurrentWeapon->WeaponData = NewWeaponData;
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
		UsingWeapon = true;
	}
	if (CurrentWeapon)
	{
		if (WeaponAmmoMemory.Contains(NewWeaponData->WeaponClass))
		{
			CurrentWeapon->CurrentAmmo = (WeaponAmmoMemory[NewWeaponData->WeaponClass]);
		}
		else
		{
			CurrentWeapon->SetCurrentAmmo();
		}
	}
}

// 헌호님 작성 코드	
FTransform UEquippableComponent::GetLeftHandSocketTransform() const
{
	if (CurrentWeapon && CurrentWeapon->WeaponMesh)
	{
		if (CurrentWeapon->WeaponMesh->DoesSocketExist(TEXT("LeftHandSocket")))
		{
			return CurrentWeapon->WeaponMesh->GetSocketTransform(TEXT("LeftHandSocket"), RTS_World);
		}
	}

	return FTransform::Identity;
}

bool UEquippableComponent::HasCurrentWeapon() const
{
	return CurrentWeapon != nullptr;
}

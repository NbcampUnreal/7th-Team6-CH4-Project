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
	
	if (CurrentWeapon->CurrentAmmo > 0)
	{
		CurrentWeapon->PlayWeaponShake();
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
	DOREPLIFETIME(UEquippableComponent, AllCurrentWeapon);
	DOREPLIFETIME(UEquippableComponent, UsingWeapon); 
}

void UEquippableComponent::Server_EquipWeapon_Implementation(UWeaponData* NewWeaponData)
{
	if (!GetOwner()->HasAuthority() || !NewWeaponData)
	{;
		return;
	}


	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (CurrentWeapon)
	{  
		UWeaponData* OldWeaponData = CurrentWeapon->WeaponData;
		WeaponAmmoMemory.Add(OldWeaponData, CurrentWeapon->CurrentAmmo); // 탄창은 총마다 저장
		SharedMaxClip = CurrentWeapon->MaxClip; // 주운 총알은 공유
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
		CurrentWeapon->SetOwner(GetOwner());
		CurrentWeapon->WeaponData = NewWeaponData;    
		CurrentWeapon->OnRep_WeaponData();              
		if (CurrentWeapon->WeaponMesh && NewWeaponData->StaticMesh)
		{
			CurrentWeapon->WeaponMesh->SetStaticMesh(NewWeaponData->StaticMesh);
		}
		CurrentWeapon->ForceNetUpdate();
	}
	
	if (CurrentWeapon)	
	{
		ACharacter* MyCharacter = Cast<ACharacter>(GetOwner());
		if (MyCharacter)
		{
			CurrentWeapon->AttachToComponent
			(MyCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale ,
			FName("Weapon_r")
			);
			CurrentWeapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CurrentWeapon->WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			CurrentWeapon->bCollisionDisabled = true;
		}
		UsingWeapon = true;
	}
	if (CurrentWeapon)
	{
		CurrentWeapon->MaxClip = SharedMaxClip; 
		if (WeaponAmmoMemory.Contains(NewWeaponData))
		{
			CurrentWeapon->CurrentAmmo = WeaponAmmoMemory[NewWeaponData];
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

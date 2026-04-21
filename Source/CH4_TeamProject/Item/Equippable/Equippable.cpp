// Fill out your copyright notice in the Description page of Project Settings.


#include "Equippable.h"

#include "WeaponData.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AEquippable::AEquippable()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = MeshComponent;
	if (WeaponMesh)
	{
		WeaponMesh->SetIsReplicated(true);
	}

	bReplicates = true;
	SetReplicateMovement(true);
}

void AEquippable::OnRep_WeaponData()
{	
	UE_LOG(LogTemp, Error, TEXT("OnRep_WeaponData 호출됨! HasAuthority: %d"), HasAuthority());
	if (!WeaponData || !WeaponMesh) return;
	
		WeaponMesh->SetStaticMesh(WeaponData->StaticMesh);
		
		WeaponMesh->SetRelativeLocation(FVector::ZeroVector);
		WeaponMesh->SetRelativeRotation(FRotator::ZeroRotator);
		WeaponMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		
		WeaponMesh->SetVisibility(true, true);
		WeaponMesh->SetHiddenInGame(false);
		WeaponMesh->MarkRenderStateDirty();
        
		UE_LOG(LogTemp, Warning, TEXT("야후! [%s] 메쉬 강제 등록 완료!"), *WeaponData->StaticMesh->GetName());
	
}

// Called when the game starts or when spawned
void AEquippable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEquippable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEquippable::Attack_Implementation_Internal()
{
}

void AEquippable::Reload_Implementation_Internal()
{
}

void AEquippable::Server_Reload_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("실제 클래스: %s"), *GetClass()->GetName());
	Reload_Implementation_Internal();
}

void AEquippable::Server_Attack_Implementation()
{
	Attack_Implementation_Internal();
	UE_LOG(LogTemp, Error, TEXT("자식꺼 공격 오버라이드 가능?"))
}


void AEquippable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEquippable, WeaponData);
	DOREPLIFETIME(AEquippable, DataAsset);
	DOREPLIFETIME(AEquippable, bCollisionDisabled);
}

void AEquippable::SetCurrentAmmo()
{
	if (WeaponData)
	{
		CurrentAmmo = WeaponData->GetMaxAmmo();
	}
}

void AEquippable::OnRep_WeaponMeshCollision()
{
	if (WeaponMesh)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

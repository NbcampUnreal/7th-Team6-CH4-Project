// Fill out your copyright notice in the Description page of Project Settings.


#include "Equippable.h"

#include "GameFramework/Character.h"


// Sets default values
AEquippable::AEquippable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

void AEquippable::Equip(ACharacter* TargetCharacter)
{
	if (TargetCharacter && HasAuthority())
	{
		// 1. 소유자 설정 (이전에 했던 Owner 설정!)
		SetOwner(TargetCharacter);

		// 2. 물리 시뮬레이션 등이 있다면 끄기
		// Mesh->SetSimulatePhysics(false);

		// 3. 캐릭터 소켓에 부착
		AttachToComponent(
			TargetCharacter->GetMesh(), 
			FAttachmentTransformRules::SnapToTargetIncludingScale
			//
		);
        
		UE_LOG(LogTemp, Warning, TEXT("%s 장착 완료!"), *GetName());
	}
}



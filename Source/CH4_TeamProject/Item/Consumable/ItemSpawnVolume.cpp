// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

AItemSpawnVolume::AItemSpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// 박스 컴포넌트 생성 및 루트 설정
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	RootComponent = SpawnBox;
}

void AItemSpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

FVector AItemSpawnVolume::GetRandomPointInVolume() const
{
	if (!SpawnBox) return GetActorLocation();
	
	FVector Origin = SpawnBox->Bounds.Origin;
	FVector BoxExtent = SpawnBox->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, BoxExtent);
}

FVector AItemSpawnVolume::GetSafeSpawnLocation()
{
	int32 MaxAttempts = 20; 
	
	for (int32 Attempt = 0; Attempt < MaxAttempts; Attempt++)
	{
		FVector CandidateLoc = GetRandomPointInVolume();
		
		FCollisionShape MySphere = FCollisionShape::MakeSphere(SafeRadius);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this); 
		
		bool bHasOverlap = GetWorld()->OverlapAnyTestByChannel(
			CandidateLoc,
			FQuat::Identity,
			ECC_Visibility,
			MySphere,
			QueryParams
		);
		
		if (!bHasOverlap)
		{
			return CandidateLoc;
		}
	}
	
	return GetRandomPointInVolume();
}

void AItemSpawnVolume::SpawnAllItems()
{
	if (!HasAuthority() || NormalItems.Num() < 2 || !SpecialItemClass) {
		UE_LOG(LogTemp,Error,TEXT("서버진입못함"))
		return;
	}
	for (int32 i = 0; i < 3; i++)
	{
		FVector SpawnLoc = GetSafeSpawnLocation();
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		GetWorld()->SpawnActor<AActor>(SpecialItemClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
	}
	
	for (int32 i = 0; i < 200; i++)
	{
		FVector SpawnLoc = GetSafeSpawnLocation();
		
		int32 RandomIndex = FMath::RandRange(0, 1);
		TSubclassOf<AActor> SelectedClass = NormalItems[RandomIndex];

		if (SelectedClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
			GetWorld()->SpawnActor<AActor>(SelectedClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("야후: 아이템 100개 스폰 완료! (기어 3개 포함)"));
}


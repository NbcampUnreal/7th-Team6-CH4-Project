#include "Monster.h"

#include "Components/CapsuleComponent.h"

AMonster::AMonster()
{
	PrimaryActorTick.bCanEverTick = false;
	
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
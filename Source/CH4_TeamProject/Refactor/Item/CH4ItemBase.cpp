#include "CH4ItemBase.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ACH4ItemBase::ACH4ItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComponent->SetIsReplicated(true);

	Tags.Add(TEXT("PickupItem"));
}

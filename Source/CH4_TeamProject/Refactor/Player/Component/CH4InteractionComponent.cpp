#include "CH4InteractionComponent.h"

#include "CH4SkillComponent.h"
#include "CH4_TeamProject/Refactor/Item/CH4ItemBase.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UCH4InteractionComponent::UCH4InteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UCH4InteractionComponent::Interact()
{
	const ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return;

	if (OwnerChar->HasAuthority())
	{
		Server_Interact_Implementation();
	}
	else
	{
		Server_Interact();
	}
}

void UCH4InteractionComponent::Server_Interact_Implementation()
{
	if (TryReviveNearbyPlayer()) return;
	if (TryPickupNearbyItem()) return;
}

bool UCH4InteractionComponent::TryReviveNearbyPlayer()
{
	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerChar);

	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		World,
		OwnerChar->GetActorLocation(),
		InteractionRadius,
		ObjectTypes,
		ACharacter::StaticClass(),
		IgnoreActors,
		OutActors);

	if (!bHit) return false;

	for (AActor* Actor : OutActors)
	{
		ACharacter* OtherCharacter = Cast<ACharacter>(Actor);
		if (!OtherCharacter) continue;

		if (OtherCharacter->ActorHasTag(TEXT("DownedPlayer")))
		{
			OnReviveRequested.Broadcast(OtherCharacter);
			return true;
		}
	}

	return false;
}

bool UCH4InteractionComponent::TryPickupNearbyItem()
{
	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerChar);

	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		World,
		OwnerChar->GetActorLocation(),
		InteractionRadius,
		ObjectTypes,
		AActor::StaticClass(),
		IgnoreActors,
		OutActors);

	if (!bHit) return false;

	UCH4SkillComponent* Skill = FindSkillComponent();

	for (AActor* Actor : OutActors)
	{
		if (!Actor) continue;
		if (!Actor->ActorHasTag(TEXT("PickupItem"))) continue;

		ACH4ItemBase* ItemActor = Cast<ACH4ItemBase>(Actor);
		if (ItemActor && ItemActor->ItemData && Skill)
		{
			Skill->ApplyItemEffect(ItemActor->ItemData);
		}

		OnPickupRequested.Broadcast();
		Actor->Destroy();
		return true;
	}

	return false;
}

ACharacter* UCH4InteractionComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

UCH4SkillComponent* UCH4InteractionComponent::FindSkillComponent() const
{
	const AActor* Owner = GetOwner();
	return Owner ? Owner->FindComponentByClass<UCH4SkillComponent>() : nullptr;
}

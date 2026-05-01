#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CH4InteractionComponent.generated.h"

class ACharacter;
class UCH4SkillComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCH4OnPickupRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCH4OnReviveRequested, ACharacter*, TargetCharacter);

UCLASS(ClassGroup = (CH4), meta = (BlueprintSpawnableComponent))
class CH4_TEAMPROJECT_API UCH4InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCH4InteractionComponent();

	void Interact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float InteractionRadius = 200.0f;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FCH4OnPickupRequested OnPickupRequested;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FCH4OnReviveRequested OnReviveRequested;

protected:
	UFUNCTION(Server, Reliable)
	void Server_Interact();

	bool TryReviveNearbyPlayer();
	bool TryPickupNearbyItem();

private:
	ACharacter* GetOwnerCharacter() const;
	UCH4SkillComponent* FindSkillComponent() const;
};

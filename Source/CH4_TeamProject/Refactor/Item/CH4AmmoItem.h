#pragma once

#include "CoreMinimal.h"
#include "CH4ItemBase.h"
#include "CH4AmmoItem.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4AmmoItem : public ACH4ItemBase
{
	GENERATED_BODY()

public:
	ACH4AmmoItem();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;
};

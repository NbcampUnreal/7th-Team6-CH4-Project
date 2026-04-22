
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FinalWaveBeginActor.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API AFinalWaveBeginActor : public AActor
{
	GENERATED_BODY()

public:
	AFinalWaveBeginActor();

protected:
	virtual void BeginPlay() override;
	

	
public:
	virtual void Tick(float DeltaTime) override;
};

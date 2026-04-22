
#include "FinalWaveBeginActor.h"


AFinalWaveBeginActor::AFinalWaveBeginActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFinalWaveBeginActor::BeginPlay()
{
	Super::BeginPlay();
	
}



void AFinalWaveBeginActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


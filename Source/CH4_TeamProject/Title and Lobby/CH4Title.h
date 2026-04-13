
#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Game/CH4GameMode.h"
#include "CH4Title.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ACH4Title : public ACH4GameMode
{
	GENERATED_BODY()
	
private:
	ACH4Title();
	
public:
	UFUNCTION(BlueprintCallable)
	void EnjoyToServerLobby();
};

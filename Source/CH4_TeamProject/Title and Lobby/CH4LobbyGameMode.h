
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CH4LobbyGameMode.generated.h"


UCLASS()
class CH4_TEAMPROJECT_API ACH4LobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
private:
	ACH4LobbyGameMode();
	
public:
	virtual void BeginPlay() override;
	
	int32 ReadyPlayerCount=0;
	
	UPROPERTY()
	int32 MaxPlayerCount = 2;
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerLifeState(ACH4PlayerController* PC);
	
	UFUNCTION(BlueprintCallable)
	void SetReadyPlayerCount();
	
	
};
	
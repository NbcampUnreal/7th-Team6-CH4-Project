
#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"
#include "LobbyPlayerController.generated.h"

UCLASS()
class CH4_TEAMPROJECT_API ALobbyPlayerController : public ACH4PlayerController
{
	GENERATED_BODY()
private:
	ALobbyPlayerController();
	
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My UI")
	TSubclassOf<UUserWidget> HUDServerLobbyWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "My UI")
	UUserWidget* HUDServerLobbyWidgetInstance;
	
	UFUNCTION(Server, Reliable,BlueprintCallable)
	void Server_RequestReady();
	
	virtual void HideCurrentWidget() override;
	
	virtual void ShowGameRule() override;
};
	

#pragma once

#include "CoreMinimal.h"
#include "CH4_TeamProject/Player/CH4PlayerController.h"
#include "TitlePlayerController.generated.h"


UCLASS()
class CH4_TEAMPROJECT_API ATitlePlayerController : public ACH4PlayerController
{
	GENERATED_BODY()
private:
	ATitlePlayerController();
	
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My UI")
	TSubclassOf<UUserWidget> HUDTitleWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "My UI")
	UUserWidget* HUDTitleWidgetInstance;
	
	
};

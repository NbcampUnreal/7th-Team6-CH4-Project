
#include "TitlePlayerController.h"

ATitlePlayerController::ATitlePlayerController()
{
	

}

void ATitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController()) return;

	if (!HUDTitleWidgetClass) 
	{
		UE_LOG(LogTemp, Error, TEXT("HUDTitleWidgetClass is nullptr!"));
		return;
	}
	
	HUDTitleWidgetInstance = CreateWidget<UUserWidget>(this,HUDTitleWidgetClass);
	if (HUDTitleWidgetInstance)
	HUDTitleWidgetInstance->AddToViewport();
}

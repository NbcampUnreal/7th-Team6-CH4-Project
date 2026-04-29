#include "CH4TitlePlayerController.h"

#include "Blueprint/UserWidget.h"

void ACH4TitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	if (!HUDTitleWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDTitleWidgetClass is null"));
		return;
	}

	HUDTitleWidgetInstance = CreateWidget<UUserWidget>(this, HUDTitleWidgetClass);
	if (HUDTitleWidgetInstance)
	{
		HUDTitleWidgetInstance->AddToViewport();
		bShowMouseCursor = true;
	}
}

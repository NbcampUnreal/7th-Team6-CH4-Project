#include "CH4HUDBase.h"

#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"

void ACH4HUDBase::DrawHUD()
{
	Super::DrawHUD();

	if (!CrosshairTexture || !Canvas) return;

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const FVector2D DrawPosition(
		Center.X - (CrosshairTexture->GetSurfaceWidth() * 0.5f),
		Center.Y - (CrosshairTexture->GetSurfaceHeight() * 0.5f));

	FCanvasTileItem TileItem(DrawPosition, CrosshairTexture->GetResource(), FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}

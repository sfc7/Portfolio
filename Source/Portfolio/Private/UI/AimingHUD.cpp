// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AimingHUD.h"
#include "Kismet/GameplayStatics.h"

void AAimingHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScale = CrossHairSpreadMax * CrossHairTexture.CrossHairSpread;

		if (CrossHairTexture.CrossHairCenter) {
			FVector2D Spread(0.f, 0.f);
			DrawCrossHair(CrossHairTexture.CrossHairCenter, ViewportCenter, Spread, CrossHairTexture.CrossHairColor);
		}
		if (CrossHairTexture.CrossHairLeft) {
			FVector2D Spread(-SpreadScale, 0.f);
			DrawCrossHair(CrossHairTexture.CrossHairLeft, ViewportCenter, Spread, CrossHairTexture.CrossHairColor);
		}
		if (CrossHairTexture.CrossHairRight) {
			FVector2D Spread(SpreadScale, 0.f);
			DrawCrossHair(CrossHairTexture.CrossHairRight, ViewportCenter, Spread, CrossHairTexture.CrossHairColor);
		}
		if (CrossHairTexture.CrossHairTop) {
			FVector2D Spread(0.f, -SpreadScale);
			DrawCrossHair(CrossHairTexture.CrossHairTop, ViewportCenter, Spread, CrossHairTexture.CrossHairColor);
		}
		if (CrossHairTexture.CrossHairBottom) {
			FVector2D Spread(0.f, SpreadScale);
			DrawCrossHair(CrossHairTexture.CrossHairBottom, ViewportCenter, Spread, CrossHairTexture.CrossHairColor);
		}
	}

}

void AAimingHUD::DrawCrossHair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrossHairColor)
{
	if (Texture == nullptr) return;

	float TextureWidth = Texture->GetSizeX();
	float TextureHeight = Texture->GetSizeY();
	FVector2D TextureDrawPoint(ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f, 0.f, 1.f, 1.f,
		CrossHairColor
	);

}


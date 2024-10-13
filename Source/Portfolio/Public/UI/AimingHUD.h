// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AimingHUD.generated.h"


USTRUCT(BlueprintType)
struct FCrossHairTexture
{
	GENERATED_BODY()
public:
		class UTexture2D* CrossHairCenter;
		UTexture2D* CrossHairLeft;
		UTexture2D* CrossHairRight;
		UTexture2D* CrossHairTop;
		UTexture2D* CrossHairBottom;
		float CrossHairSpread;
		FLinearColor CrossHairColor;

		FCrossHairTexture() :
			CrossHairCenter(nullptr),
			CrossHairLeft(nullptr),
			CrossHairRight(nullptr),
			CrossHairTop(nullptr),
			CrossHairBottom(nullptr),
			CrossHairSpread(0.0f),
			CrossHairColor(FLinearColor::White)
		{};
};
/**
 * 
 */
UCLASS()
class PORTFOLIO_API AAimingHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	void SetCrossHairTexture(const FCrossHairTexture& _CrossHairTexture) { CrossHairTexture = _CrossHairTexture; }

private:
	void DrawCrossHair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrossHairColor);

private:
	FCrossHairTexture CrossHairTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float CrossHairSpreadMax = 16.f;
};

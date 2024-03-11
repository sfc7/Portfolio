// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PlayerWidget.h"
#include "Player_Bar.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayer_Bar : public UPlayerWidget
{
	GENERATED_BODY()
public:
	UPlayer_Bar(const FObjectInitializer& ObjectInitializer);

	void SetMaxFigure(float _MaxFigure);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<class UProgressBar> Bar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float MaxFigure;
};

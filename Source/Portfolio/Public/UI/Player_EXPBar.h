// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Player_Bar.h"
#include "Player_EXPBar.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayer_EXPBar : public UPlayer_Bar
{
	GENERATED_BODY()
public:
	void SetMaxExp(float _MaxEXP);

	void InitalizeEXPBarWidget(class AFPlayerState* NewPlayerState);

	UFUNCTION()
		void OnCurrentEXPChange(float NewEXP);

	UFUNCTION()
		void OnMaxEXPChange(float NewMaxEXP);

protected:
	virtual void NativeConstruct() override;


	
};

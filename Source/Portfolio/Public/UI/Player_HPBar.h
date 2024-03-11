// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Player_Bar.h"
#include "Player_HPBar.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayer_HPBar : public UPlayer_Bar
{
	GENERATED_BODY()
public:
	void SetMaxHp(float _MaxHp);

	void InitalizeHPBarWidget(class UCharacterComponent* NewComponent);

	UFUNCTION()
		void OnMaxHpChange(float _MaxHp, float NewMaxHp);

	UFUNCTION()
		void OnCurrentHpChange(float _Hp, float NewHp);

protected:
	virtual void NativeConstruct() override;
};

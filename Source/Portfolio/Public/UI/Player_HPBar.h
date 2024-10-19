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
	void SetMaxHp(int _MaxHp);

	void InitalizeHPBarWidget(class UCharacterComponent* NewComponent);

	UFUNCTION()
		void OnMaxHpChange(int _MaxHp, int NewMaxHp);

	UFUNCTION()
		void OnCurrentHpChange(int _Hp, int NewHp);

protected:
	virtual void NativeConstruct() override;
};

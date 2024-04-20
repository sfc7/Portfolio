// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	void BindCharacterComponent(class UCharacterComponent* _CharacterComponent);

	void BindPlayerState(class AFPlayerState* _PlayerState);

protected:
	UFUNCTION()
		virtual void LevelTextChange(int32 NewLevel);

	UFUNCTION()
		virtual void MoneyChange(int32 _Money);

protected:
	TWeakObjectPtr<class UCharacterComponent> CharacterComponent;

	TWeakObjectPtr<class AFPlayerState> PlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UTextBlock> LevelText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UTextBlock> Money;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UPlayer_HPBar> Hp_Bar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UPlayer_EXPBar> Exp_Bar;

};	

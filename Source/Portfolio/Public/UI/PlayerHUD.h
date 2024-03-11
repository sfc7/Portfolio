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

protected:
	TWeakObjectPtr<class UCharacterComponent> CharacterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UTextBlock> LevelText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UPlayer_HPBar> Hp_Bar;

};

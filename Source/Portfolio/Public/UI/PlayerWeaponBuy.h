// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWeaponBuy.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayerWeaponBuy : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateWidget(struct FInteractableData* InteractableData);

protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();
		
	UFUNCTION()
	float UpdateInteractionProgress();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateaccess = true))
		TObjectPtr<class UTextBlock> WeaponName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateaccess = true))
		TObjectPtr<class UTextBlock> WeaponPrice;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateaccess = true))
		TObjectPtr<class UProgressBar> InteractionProgressBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateaccess = true))
		float CurrentInteractionDuration;
};

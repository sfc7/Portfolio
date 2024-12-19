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
	void UpdateWidget(struct FWeaponData* PurchasableWeaponData);

protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateaccess = true))
		TObjectPtr<class UTextBlock> WeaponName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateaccess = true))
		TObjectPtr<class UTextBlock> WeaponPrice;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerWeaponBuy.h"
#include "Components/TextBlock.h"
#include "Interface/InteractionInterface.h"
#include "Data/DataStruct.h"
#include "Kismet/GameplayStatics.h"


void UPlayerWeaponBuy::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPlayerWeaponBuy::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerWeaponBuy::UpdateWidget(FPurchasableWeaponData* PurchasableWeaponData)
{	
	WeaponName->SetText(PurchasableWeaponData->Name);
	FString ConvertString = FString::Printf(TEXT("[$:%d]"), PurchasableWeaponData->Price);
	WeaponPrice->SetText(FText::FromString(ConvertString));
}


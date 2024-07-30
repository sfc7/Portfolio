// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerWeaponBuy.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Interface/InteractionInterface.h"


void UPlayerWeaponBuy::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InteractionProgressBar->PercentDelegate.BindUFunction(this, "UpdateInteractionProgress");
}

void UPlayerWeaponBuy::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentInteractionDuration = 0.0f;
}

float UPlayerWeaponBuy::UpdateInteractionProgress()
{
	return 0.0f;
}

void UPlayerWeaponBuy::UpdateWidget(FInteractableData* InteractableData)
{
	switch (InteractableData->InteractableType)
	{
		case EInteractableType::PickUp:
			break;
		default:;
	}

	WeaponName->SetText(InteractableData->Name);
	FString ConvertString = FString::Printf(TEXT("[$:%f]"), InteractableData->Price);
	WeaponPrice->SetText(FText::FromString(ConvertString));

}
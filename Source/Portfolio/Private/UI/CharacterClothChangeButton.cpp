// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterClothChangeButton.h"
#include "Components/Button.h"

UCharacterClothChangeButton::UCharacterClothChangeButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UCharacterClothChangeButton::NativeConstruct()
{
	Super::NativeConstruct();

	ClothChangeButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnClothChangeButtonClicked);
}

void UCharacterClothChangeButton::OnClothChangeButtonClicked()
{

}

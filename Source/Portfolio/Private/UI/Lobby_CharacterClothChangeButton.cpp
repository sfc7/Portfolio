// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby_CharacterClothChangeButton.h"
#include "Slate/SlateBrushAsset.h"
#include "Components/Button.h"
#include "Components/Image.h"

ULobby_CharacterClothChangeButton::ULobby_CharacterClothChangeButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULobby_CharacterClothChangeButton::NativeConstruct()
{
	Super::NativeConstruct();



	ClothChangeButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnClothChangeButtonClicked);
}

void ULobby_CharacterClothChangeButton::OnClothChangeButtonClicked()
{
}

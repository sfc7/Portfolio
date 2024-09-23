// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterClothChangeButton.h"
#include "Slate/SlateBrushAsset.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Character/LobbyCharacter.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

UCharacterClothChangeButton::UCharacterClothChangeButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UCharacterClothChangeButton::NativeConstruct()
{
	Super::NativeConstruct();


    if (IsValid(ClothChangeButton) && IsValid(ClothChangeTexture))
    {
        ClothChangeImage->SetBrushFromTexture(ClothChangeTexture);
    }

	ClothChangeButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnClothChangeButtonClicked);
    ClothChangeButton.Get()->OnHovered.AddDynamic(this, &ThisClass::OnClothChangeButtonHovered);
    ClothChangeButton.Get()->OnUnhovered.AddDynamic(this, &ThisClass::OnClothChangeButtonUnHovered);
}

void UCharacterClothChangeButton::OnClothChangeButtonClicked()
{
    ALobbyCharacter* PlayerCharacter = Cast<ALobbyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

    if (IsValid(PlayerCharacter) && IsValid(ClothChangeSkeletalMesh)) {
        PlayerCharacter->SetSkeletalMeshInPlayerStateSave(ClothChangeSkeletalMesh);
    }
    
}

void UCharacterClothChangeButton::OnClothChangeButtonHovered()
{
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ClothChangeButton->Slot))
    {
        CanvasSlot->SetSize(FVector2D(170.f, 230.f));
        ClothChangeImage->SetBrushSize(FVector2D(170.f, 230.f));
    }
}

void UCharacterClothChangeButton::OnClothChangeButtonUnHovered()
{
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ClothChangeButton->Slot))
    {
        CanvasSlot->SetSize(FVector2D(150.f, 200.f));
        ClothChangeImage->SetBrushSize(FVector2D(150.f, 200.f));
    }
}

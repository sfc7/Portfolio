// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterClothChangeButton.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UCharacterClothChangeButton : public UUserWidget
{
	GENERATED_BODY()
public:
	UCharacterClothChangeButton(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnClothChangeButtonClicked();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UButton> ClothChangeButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UImage> ClothChangeImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UTexture2D* ClothChangeTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class USkeletalMesh* ClothChangeSkeletalMesh;
};

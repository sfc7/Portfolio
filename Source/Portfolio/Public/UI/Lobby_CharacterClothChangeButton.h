// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Lobby_UI.h"
#include "Lobby_CharacterClothChangeButton.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ULobby_CharacterClothChangeButton : public ULobby_UI
{
	GENERATED_BODY()
public:
	ULobby_CharacterClothChangeButton(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnClothChangeButtonClicked();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UButton> ClothChangeButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		TObjectPtr<class UImage> ClothChangeImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UTexture2D* ClothChangeTexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class USkeletalMesh* ClothChangeSkeletalMesh;

};

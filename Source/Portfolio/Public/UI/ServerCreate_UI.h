// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerCreate_UI.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UServerCreate_UI : public UUserWidget
{
	GENERATED_BODY()
public:
    UServerCreate_UI(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
        void OnNewGameButtonClicked();

    UFUNCTION()
        void OnSaveGameButtonClicked();

    UFUNCTION()
        void OnBackButtonClicked();


private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
        TObjectPtr<class UButton> NewGameButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
        TObjectPtr<class UButton> SaveGameButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
        TObjectPtr<class UButton> BackGameButton;
};

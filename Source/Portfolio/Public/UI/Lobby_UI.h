// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Lobby_UI.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ULobby_UI : public UUserWidget
{
	GENERATED_BODY()
public:
    ULobby_UI(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;

private:

    UFUNCTION()
        void OnSubmitButtonClicked();

    UFUNCTION()
        void OnPreviousButtonClicked();


    void SaveInitializedSaveData();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
        TObjectPtr<class UButton> SubmitButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
        TObjectPtr<class UButton> PreviousButton;

    FString PlayerName = TEXT("DefaultPlayerName");
};
    
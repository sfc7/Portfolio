// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Title_UI.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UTitle_UI : public UUserWidget
{
	GENERATED_BODY()
public:
	UTitle_UI(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
        void OnJoinGameButtonClicked();

    UFUNCTION()
        void OnExitGameButtonClicked();

private:
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
        TObjectPtr<class UButton> JoinGameButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
        TObjectPtr<class UButton> SettingsButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
        TObjectPtr<class UButton> ExitGameButton;
};

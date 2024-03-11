// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu_UI.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UMenu_UI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnResumeButtonClicked();

	UFUNCTION()
		void OnTitleButtonClicked();

	UFUNCTION()
		void OnExitButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
		TObjectPtr<class UButton> ResumeButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
		TObjectPtr<class UButton> TitleButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess, BindWidget))
		TObjectPtr<class UButton> ExitButton;

};

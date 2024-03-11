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
        void OnRedTeamButtonClicked();

    UFUNCTION()
        void OnBlueTeamButtonClicked();

    UFUNCTION()
        void OnSubmitButtonClicked();

    UFUNCTION()
        void OnTestButtonClicked();

    void SaveInitializedSaveData();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
        TObjectPtr<class UButton> RedTeamButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
        TObjectPtr<class UButton> BlueTeamButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
        TObjectPtr<class UEditableText> EditPlayerName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
        TObjectPtr<class UButton> SubmitButton;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
        TObjectPtr<class UButton> TestButton;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USkeletalMesh* RedTeamMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USkeletalMesh* BlueTeamMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
        class USkeletalMesh* CurrentSkeletalMesh;

    TWeakObjectPtr<class USkeletalMeshComponent> CurrentSkeletalMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess, BindWidget))
        TObjectPtr<class UEditableText> EditServerIP;

    FString PlayerName = TEXT("DefaultPlayerName");
};
    
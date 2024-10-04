// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby_UI.h"
#include "Character/FCharacter.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/UIController.h"
#include "Portfolio/Portfolio.h"
#include "GameFramework/PlayerState.h"
#include "Character/LobbyCharacter.h"
#include "Game/FPlayerState.h"
#include "Game/PlayerStateSave.h"
#include "Engine/Engine.h"

ULobby_UI::ULobby_UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ULobby_UI::NativeConstruct()
{
    Super::NativeConstruct();

   //PreviousButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPreviousButtonClicked);
   //SubmitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnSubmitButtonClicked);
}

void ULobby_UI::OnSubmitButtonClicked()
{   
    //SaveInitializedSaveData();

    //AUIController* PlayerUIController = GetOwningPlayer<AUIController>();
    //if (IsValid(PlayerUIController))
    //{
    //    PlayerUIController->JoinServer(FString(TEXT("127.0.0.1:17777")));
    //}
}

void ULobby_UI::OnPreviousButtonClicked()
{
    //AUIController* PlayerUIController = GetOwningPlayer<AUIController>();
    //if (IsValid(PlayerUIController))
    //{
    //    PlayerUIController->JoinServer(FString(TEXT("Room")));
    //}
}

void ULobby_UI::SaveInitializedSaveData()
{
    /* FText ServerIP = EditServerIP->GetText();*/
    /* PlayerUIController->JoinServer(ServerIP.ToString());*/
}
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

ULobby_UI::ULobby_UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    //static ConstructorHelpers::FObjectFinder<USkeletalMesh> RedTeamMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
    //if (RedTeamMeshAsset.Succeeded()) {
    //    RedTeamMesh = RedTeamMeshAsset.Object;
    //}

    //static ConstructorHelpers::FObjectFinder<USkeletalMesh> BlueTeamMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin_2.SK_Mannequin_2'"));
    //if (BlueTeamMeshAsset.Succeeded()) {
    //    BlueTeamMesh = BlueTeamMeshAsset.Object;
    //}

}

void ULobby_UI::NativeConstruct()
{
    Super::NativeConstruct();

    AFCharacter* FCharacter = Cast<AFCharacter>(GetOwningPlayerPawn());
    CurrentSkeletalMeshComponent = FCharacter->GetMesh();

    SubmitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnSubmitButtonClicked);

}

//void ULobby_UI::OnRedTeamButtonClicked()
//{
//    UPlayerStateSave* PlayerStateSave = NewObject<UPlayerStateSave>();
//    PlayerStateSave->PlayerCharacterName = PlayerName;
//
//    UGameplayStatics::SaveGameToSlot(PlayerStateSave, FString::FromInt(GPlayInEditorID), 0);
//}
//
//void ULobby_UI::OnBlueTeamButtonClicked()
//{
//    if (IsValid(BlueTeamMesh)) {
//        
//        ALobbyCharacter* LobbyCharacter = Cast<ALobbyCharacter>(GetOwningPlayerPawn());
//        if (IsValid(LobbyCharacter)) {
//            LobbyCharacter->SetSkeletalMeshInPlayerStateSave(BlueTeamMesh);
//        }   
//    }
//}

void ULobby_UI::OnSubmitButtonClicked()
{   
    
    SaveInitializedSaveData();

    //AUIController* PlayerUIController = GetOwningPlayer<AUIController>();
    //if (IsValid(PlayerUIController))
    //{
    //    FText ServerIP = EditServerIP->GetText();
    //    PlayerUIController->JoinServer(ServerIP.ToString());

    //}
}

void ULobby_UI::OnPreviousButtonClicked()
{
}

void ULobby_UI::SaveInitializedSaveData()
{
    /* FText ServerIP = EditServerIP->GetText();*/
    /* PlayerUIController->JoinServer(ServerIP.ToString());*/
}
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
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> RedTeamMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));
    if (RedTeamMeshAsset.Succeeded()) {
        RedTeamMesh = RedTeamMeshAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> BlueTeamMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin_2.SK_Mannequin_2'"));
    if (BlueTeamMeshAsset.Succeeded()) {
        BlueTeamMesh = BlueTeamMeshAsset.Object;
    }

}

void ULobby_UI::NativeConstruct()
{
    Super::NativeConstruct();

    AFCharacter* FCharacter = Cast<AFCharacter>(GetOwningPlayerPawn());
    CurrentSkeletalMeshComponent = FCharacter->GetMesh();

    RedTeamButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnRedTeamButtonClicked);
    BlueTeamButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnBlueTeamButtonClicked);
    SubmitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnSubmitButtonClicked);
    TestButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnTestButtonClicked);

}

void ULobby_UI::OnRedTeamButtonClicked()
{
    UPlayerStateSave* PlayerStateSave = NewObject<UPlayerStateSave>();
    PlayerStateSave->PlayerCharacterName = PlayerName;

    UGameplayStatics::SaveGameToSlot(PlayerStateSave, FString::FromInt(GPlayInEditorID), 0);
}

void ULobby_UI::OnBlueTeamButtonClicked()
{
    if (IsValid(BlueTeamMesh)) {
        
        ALobbyCharacter* LobbyCharacter = Cast<ALobbyCharacter>(GetOwningPlayerPawn());
        if (IsValid(LobbyCharacter)) {
            LobbyCharacter->SetSkeletalMeshInPlayerStateSave(BlueTeamMesh);
        }
    }
}

void ULobby_UI::OnSubmitButtonClicked()
{   
    PlayerName = EditPlayerName->GetText().ToString();
    if (PlayerName.Len() <= 0 || 10 <= PlayerName.Len()) {
        return;
    }
    
    SaveInitializedSaveData();

    AUIController* PlayerUIController = GetOwningPlayer<AUIController>();
    if (IsValid(PlayerUIController)) {
        GetWorld()->ServerTravel(TEXT("/Game/FirstPerson/Maps/FirstPersonMap?listen"));
    }
}

void ULobby_UI::OnTestButtonClicked()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Title")));
}

void ULobby_UI::SaveInitializedSaveData()
{
    /* FText ServerIP = EditServerIP->GetText();*/
    /* PlayerUIController->JoinServer(ServerIP.ToString());*/
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title_UI.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/UIController.h"
#include "Game/PlayerStateSave.h"

UTitle_UI::UTitle_UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UTitle_UI::NativeConstruct()
{
	JoinGameButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnJoinGameButtonClicked);
	ExitGameButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitGameButtonClicked);
}

void UTitle_UI::OnJoinGameButtonClicked()
{
    //APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    //if (PlayerController->HasAuthority())
    //{
    //     서버인 경우
    //    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Lobby")));
    //}
    //else
    //{
    //     클라이언트인 경우
    //    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("\127.0.0.1:7777")));
    //}

    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Lobby")));
}

void UTitle_UI::OnExitGameButtonClicked()		
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}	

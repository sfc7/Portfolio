// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title_UI.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Game/PlayerStateSave.h"

UTitle_UI::UTitle_UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UTitle_UI::NativeConstruct()
{
	CreateGameButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnCreateGameButtonClicked);
	JoinGameButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnJoinGameButtonClicked);
	ExitGameButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitGameButtonClicked);
}

void UTitle_UI::OnCreateGameButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=127.0.0.1:17777")));
}

void UTitle_UI::OnJoinGameButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=ServerJoin")));
}

void UTitle_UI::OnExitGameButtonClicked()		
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

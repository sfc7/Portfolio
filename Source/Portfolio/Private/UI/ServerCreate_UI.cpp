// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ServerCreate_UI.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Game/FPlayerState.h"
#include "Game/PlayerStateSave.h"

UServerCreate_UI::UServerCreate_UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UServerCreate_UI::NativeConstruct()
{
	NewGameButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnNewGameButtonClicked);
	SaveGameButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnSaveGameButtonClicked);
	BackGameButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnBackButtonClicked);

	SaveGameButton->SetIsEnabled(false);
	const AFPlayerState* FPlayerState = GetDefault<AFPlayerState>();
	if (IsValid(FPlayerState)) {
		UPlayerStateSave* PlayerStateSave = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));
		if (IsValid(PlayerStateSave)) {
			SaveGameButton->SetIsEnabled(true);
		}
	}
}

void UServerCreate_UI::OnNewGameButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Lobby")));
}

void UServerCreate_UI::OnSaveGameButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Lobby")));
}

void UServerCreate_UI::OnBackButtonClicked()
{
}

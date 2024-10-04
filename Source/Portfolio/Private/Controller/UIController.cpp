// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/UIController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Game/FPlayerState.h"
#include "Game/FGameState.h"
#include "Portfolio/Portfolio.h"
#include "Game/LobbyGameMode.h"
#include "Character/LobbyCharacter.h"
#include "Engine/World.h"

void AUIController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController()) {
		//SpawnPlayerMove_Server();

		if (IsValid(WBP_UIClass)) {
			WBP_UI = CreateWidget<UUserWidget>(this, WBP_UIClass);
			if (IsValid(WBP_UI)) {
				WBP_UI->AddToViewport();

				FInputModeUIOnly UIMode;
				UIMode.SetWidgetToFocus(WBP_UI->GetCachedWidget());
				SetInputMode(UIMode);

				bShowMouseCursor = true;
			}
		}

		if (IsValid(LoadingScreenClass)) {
			LoadingScreen = CreateWidget<UUserWidget>(this, LoadingScreenClass);
			LoadingScreen->AddToViewport();
			LoadingScreen->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AUIController::JoinServer(const FString& IPAddress)
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString::Printf(TEXT("NextLevel=%s?Saved=false"), *IPAddress));
}

void AUIController::EndMap()
{
	if (IsValid(LoadingScreen) && IsLocalPlayerController()) {
		LoadingScreen->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly UIMode;
		UIMode.SetWidgetToFocus(LoadingScreen->GetCachedWidget());
		SetInputMode(UIMode);

		bShowMouseCursor = true;
	}
}

void AUIController::JoinOpen_Server_Implementation()
{
	ClientTravel(*FString(TEXT("Room")), ETravelType::TRAVEL_Absolute);
}



void AUIController::SpawnPlayerMove_Server_Implementation()
{
	//ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	//if (IsValid(LobbyGameMode)) {
	//	FTransform PlayerStartTransform = LobbyGameMode->GetPlayerStartTransform();
	//	ALobbyCharacter* LobbyCharacter = this->GetPawn<ALobbyCharacter>();
	//	LobbyCharacter->SetActorLocationAndRotation(PlayerStartTransform.GetLocation(), PlayerStartTransform.GetRotation(), false, 0, ETeleportType::None);
	//}	
}


		
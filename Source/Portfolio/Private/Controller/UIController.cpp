// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/UIController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Game/FPlayerState.h"
#include "Game/FGameState.h"
#include "Portfolio/Portfolio.h"
#include "Game/LobbyGameMode.h"
#include "Character/LobbyCharacter.h"

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
	}
}

void AUIController::JoinServer(const FString& IPAddress)
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString::Printf(TEXT("NextLevel=%s?Saved=false"), *IPAddress));
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


		
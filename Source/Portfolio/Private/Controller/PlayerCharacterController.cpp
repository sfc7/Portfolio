// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PlayerCharacterController.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerHUD.h"
#include "Component/CharacterComponent.h"
#include "Character/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Portfolio/Portfolio.h"
#include "Game/MainGameMode.h"

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocalPlayerController()) {
		SpawnPlayerMove_Server();

		FInputModeGameOnly GameOnlyMode;
		SetInputMode(GameOnlyMode);

		if (IsValid(CrosshairUIClass)) {
			UUserWidget* CrosshairUI = CreateWidget<UUserWidget>(this, CrosshairUIClass);
			if (IsValid(CrosshairUI)) {
				CrosshairUI->AddToViewport(1);
				CrosshairUI->SetVisibility(ESlateVisibility::Visible);
			}
		}

		if (IsValid(HUDWidgetClass)) {
			HUDWidget = CreateWidget<UPlayerHUD>(this, HUDWidgetClass);
			if (IsValid(HUDWidget)) {
				HUDWidget->AddToViewport();

				APlayerCharacter* PlayerCharacter = GetPawn<APlayerCharacter>();
				if (IsValid(PlayerCharacter)) {
					UCharacterComponent* CharacterComponent = PlayerCharacter->GetCharacterComponent();
					if (IsValid(CharacterComponent)) {
						HUDWidget->BindCharacterComponent(CharacterComponent);
					}
				}
			}
		}

		if (IsValid(MenuUIClass)) {
			MenuUI = CreateWidget<UUserWidget>(this, MenuUIClass);
			if (IsValid(MenuUI)) {
				MenuUI->AddToViewport(3);
				MenuUI->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void APlayerCharacterController::ToggleMenu()
{
	if (IsValid(MenuUI)) {
		if (!FlagMenu) {
			MenuUI->SetVisibility(ESlateVisibility::Visible);

			FInputModeUIOnly UIMode;
			UIMode.SetWidgetToFocus(MenuUI->GetCachedWidget());
			SetInputMode(UIMode);

			bShowMouseCursor = true;
		}
		else {
			MenuUI->SetVisibility(ESlateVisibility::Collapsed);

			FInputModeGameOnly GameOnlyMode;
			SetInputMode(GameOnlyMode);

			bShowMouseCursor = false;
		}	

		FlagMenu = !FlagMenu;
	}
}

void APlayerCharacterController::SpawnPlayerMove_Server_Implementation()
{
	AMainGameMode* MainGameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(MainGameMode)) {
		FTransform PlayerStartTransform = MainGameMode->GetPlayerStartTransform();
		APlayerCharacter* PlayerCharacter = this->GetPawn<APlayerCharacter>();
		PlayerCharacter->SetActorLocationAndRotation(PlayerStartTransform.GetLocation(), PlayerStartTransform.GetRotation(), false, 0, ETeleportType::None);
	}
}

void APlayerCharacterController::PlayerBeginPlaySetMesh(USkeletalMesh* _PlayerMesh)
{
	if (IsLocalPlayerController()) {
		APlayerCharacter* PlayerCharacter = GetPawn<APlayerCharacter>();
		if (IsValid(PlayerCharacter)) {
			PlayerCharacter->SetPlayerMesh_Server(_PlayerMesh);
		}
	}
}

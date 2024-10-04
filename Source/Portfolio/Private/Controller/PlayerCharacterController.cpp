// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PlayerCharacterController.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerHUD.h"
#include "Component/CharacterComponent.h"
#include "Character/PlayerCharacter.h"
#include "Portfolio/Portfolio.h"
#include "Game/MainGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "WorldStatic/Weapon/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Game/FGameInstance.h"
#include "Game/FPlayerState.h"
#include "Interface/InteractionInterface.h"
#include "UI/PlayerWeaponBuy.h"

void APlayerCharacterController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, UserNotificationText);
	DOREPLIFETIME(ThisClass, WaveText);
}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController()) {
		SpawnPlayerMove_Server();

		FInputModeGameOnly GameOnlyMode;
		SetInputMode(GameOnlyMode);

		if (IsValid(HUDWidgetClass)) {
			HUDWidget = CreateWidget<UPlayerHUD>(this, HUDWidgetClass);

			if (IsValid(HUDWidget)) {
				HUDWidget->AddToViewport();
				APlayerCharacter* PlayerCharacter = GetPawn<APlayerCharacter>();
			}
		}

		if (IsValid(LoadingScreenClass)) {
			LoadingScreen = CreateWidget<UUserWidget>(this, LoadingScreenClass);
			if (LoadingScreen) {
				LoadingScreen->AddToViewport();
				LoadingScreen->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (IsValid(WeaponBuyWidgetClass)) {
			WeaponBuyWidget = CreateWidget<UPlayerWeaponBuy>(this, WeaponBuyWidgetClass);
			if(IsValid(WeaponBuyWidget)){
				WeaponBuyWidget->AddToViewport();
				WeaponBuyWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (IsValid(CrosshairUIClass)) {
			UUserWidget* CrosshairUI = CreateWidget<UUserWidget>(this, CrosshairUIClass);
			if (IsValid(CrosshairUI)) {
				CrosshairUI->AddToViewport(1);
				CrosshairUI->SetVisibility(ESlateVisibility::Visible);
			}
		}

		if (IsValid(UserNotificationTextUIClass)) {
			UserNotificationTextUI = CreateWidget<UUserWidget>(this, UserNotificationTextUIClass);
			if (IsValid(UserNotificationTextUI)) {
				UserNotificationTextUI->AddToViewport(1);
				UserNotificationTextUI->SetVisibility(ESlateVisibility::Visible);
			}
		}

		if (IsValid(WaveTextUIClass)) {
			WaveTextUI = CreateWidget<UUserWidget>(this, WaveTextUIClass);
			if (IsValid(WaveTextUI)) {
				WaveTextUI->AddToViewport(1);
				WaveTextUI->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}

	if (IsLocalPlayerController()) {
		AFPlayerState* FPlayerState = GetPlayerState<AFPlayerState>();
		if (IsValid(FPlayerState)) {
			FPlayerState->InitPlayerState();
		}
	}
}

void APlayerCharacterController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void APlayerCharacterController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AFPlayerState* FPlayerState = GetPlayerState<AFPlayerState>();
	if (IsValid(FPlayerState)) {
		FPlayerState->InitPlayerState();
	}

}

void APlayerCharacterController::BindPlayerState(AFPlayerState* _PlayerState)
{
	if (IsValid(HUDWidget)) {
		HUDWidget->BindPlayerState(_PlayerState);
	}
}

void APlayerCharacterController::EndMap()
{
	if (IsValid(LoadingScreen)/* && IsLocalPlayerController()*/) {
		LoadingScreen->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly UIMode;
		UIMode.SetWidgetToFocus(LoadingScreen->GetCachedWidget());
		SetInputMode(UIMode);

		bShowMouseCursor = true;
	}

	EndMap_Client();
}

void APlayerCharacterController::EndMap_Client_Implementation()
{
	if (IsValid(LoadingScreen)/* && IsLocalPlayerController()*/) {
		LoadingScreen->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly UIMode;
		UIMode.SetWidgetToFocus(LoadingScreen->GetCachedWidget());
		SetInputMode(UIMode);

		bShowMouseCursor = true;
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

void APlayerCharacterController::WeaponBuyShow(FPurchasableWeaponData* PurchasableWeaponData)
{
	if (IsValid(WeaponBuyWidget)) {
		if (WeaponBuyWidget->GetVisibility() == ESlateVisibility::Collapsed) {
			WeaponBuyWidget->SetVisibility(ESlateVisibility::Visible);
		}
		WeaponBuyWidget->UpdateWidget(PurchasableWeaponData);
	}
}

void APlayerCharacterController::WeaponBuyHide() 
{
	if (IsValid(WeaponBuyWidget)) {
		WeaponBuyWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
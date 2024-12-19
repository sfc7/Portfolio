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
#include "Game/PlayerStateSave.h"
#include "Interface/InteractionInterface.h"
#include "UI/PlayerWeaponBuy.h"

void APlayerCharacterController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, UserNotificationText);
	DOREPLIFETIME(ThisClass, WaveText);
	DOREPLIFETIME(ThisClass, HeadShotUI);
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
				LoadingScreen->AddToViewport(999);
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

		if (IsValid(HeadShotUIClass)) {
			HeadShotUI = CreateWidget<UUserWidget>(this, HeadShotUIClass);
			if (IsValid(HeadShotUI)) {
				HeadShotUI->AddToViewport(1);
				HeadShotUI->SetVisibility(ESlateVisibility::Collapsed);
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

void APlayerCharacterController::EndMap_Client_Implementation()
{
	if (IsValid(LoadingScreen)) {
		LoadingScreen->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly UIMode;
		UIMode.SetWidgetToFocus(LoadingScreen->GetCachedWidget());
		SetInputMode(UIMode);

		bShowMouseCursor = true;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (IsValid(PlayerCharacter)) {
		UPlayerStateSave* PlayerStateLoad = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));
		if (!IsValid(PlayerStateLoad)) {
			PlayerStateLoad = Cast<UPlayerStateSave>(UGameplayStatics::CreateSaveGameObject(UPlayerStateSave::StaticClass()));
		}

		PlayerStateLoad->WeaponSlotSaveData.FirstPrimaryWeaponData = PlayerCharacter->GetWeaponSlot().FirstPrimaryWeapon->GetWeaponData();
		PlayerStateLoad->WeaponSlotSaveData.SecondPrimaryWeaponData = PlayerCharacter->GetWeaponSlot().SecondPrimaryWeapon->GetWeaponData();

		UGameplayStatics::SaveGameToSlot(PlayerStateLoad, FString::FromInt(GPlayInEditorID), 0);

		UFGameInstance* FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
		if (IsValid(FGameInstance)) {
			FGameInstance->CurrentLevel = PlayerCharacter->GetCharacterComponent()->GetCurrentLevel();
			FGameInstance->CurrentEXP = PlayerCharacter->GetCharacterComponent()->GetCurrentEXP();
			FGameInstance->PlayerMoney = PlayerCharacter->GetCharacterComponent()->GetMoney();
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

void APlayerCharacterController::WeaponBuyShow(FWeaponData* PurchasableWeaponData)
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

void APlayerCharacterController::OnHeadShotUI()
{
	if (IsValid(HeadShotUI)) {
		if (HeadShotUI->GetVisibility() == ESlateVisibility::Collapsed) {
			HeadShotUI->SetVisibility(ESlateVisibility::Visible);
			GetWorld()->GetTimerManager().SetTimer(HeadShotHanldle, FTimerDelegate::CreateLambda([this]() {
				HeadShotUI->SetVisibility(ESlateVisibility::Collapsed);
		}), 2.0f, false);
		}
		else {
			HeadShotUI->SetVisibility(ESlateVisibility::Collapsed);
			HeadShotUI->SetVisibility(ESlateVisibility::Visible);
			GetWorld()->GetTimerManager().SetTimer(HeadShotHanldle, FTimerDelegate::CreateLambda([this]() {
				HeadShotUI->SetVisibility(ESlateVisibility::Collapsed);
			}), 2.0f, false);
		}
	}
	
}


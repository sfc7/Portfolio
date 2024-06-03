// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PlayerCharacterController.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerHUD.h"
#include "Component/CharacterComponent.h"
#include "Character/PlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Portfolio/Portfolio.h"
#include "Game/MainGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "WorldStatic/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Game/FGameInstance.h"
#include "Game/FPlayerState.h"

void APlayerCharacterController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, UserNotificationText);
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
				if (IsValid(PlayerCharacter)) {
					UCharacterComponent* CharacterComponent = PlayerCharacter->GetCharacterComponent();
					if (IsValid(CharacterComponent)) {
						HUDWidget->BindCharacterComponent(CharacterComponent);
					}

				AFPlayerState* FPlayerState = GetPlayerState<AFPlayerState>();
				if (IsValid(FPlayerState)) {
					BindPlayerState(FPlayerState);
					FPlayerState->InitPlayerState();
					}
				}
			}
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

	/*if (IsLocalPlayerController()) {

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

				AFPlayerState* FPlayerState = GetPlayerState<AFPlayerState>();
				if (FPlayerState) {
					BindPlayerState(FPlayerState);
					FPlayerState->InitPlayerState();
				}
			}
		}
	}*/
}

void APlayerCharacterController::BindPlayerState(AFPlayerState* _PlayerState)
{
	if (IsValid(HUDWidget)) {
		HUDWidget->BindPlayerState(_PlayerState);
	}
}

void APlayerCharacterController::LevelTransition(const FString& _LevelPath)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString::Printf(TEXT("NextLevel=%sTransitionType=ServerTravel"), *_LevelPath));
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


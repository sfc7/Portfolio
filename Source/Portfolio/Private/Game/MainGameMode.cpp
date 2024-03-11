// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MainGameMode.h"
#include "Game/FPlayerState.h"
#include "Portfolio/Portfolio.h"
#include "Character/PlayerCharacter.h"
#include "Controller/PlayerCharacterController.h"

AMainGameMode::AMainGameMode()
{
	static ConstructorHelpers::FClassFinder<APlayerCharacterController> PlayerControllerBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Controller/BP_PlayerCharacterController.BP_PlayerCharacterController_C'"));
	if (PlayerControllerBP.Succeeded())
	{
		PlayerControllerClass = PlayerControllerBP.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerCharacter> PlayerCharacterBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Actor/Character/BP_Character.BP_Character_C'"));
	if (PlayerCharacterBP.Succeeded()) {
		PlayerCharacterClass = PlayerCharacterBP.Class;
	}	

	PlayerStateClass = AFPlayerState::StaticClass();
}

//void AMainGameMode::SpawnPlayer(APlayerController* _PlayerController)
//{
//	Super::SpawnPlayer(_PlayerController);
//
//	if (HasAuthority()) {
//		APlayerCharacter* PlayerCharacter = _PlayerController->GetPawn<APlayerCharacter>();
//		if (IsValid(PlayerCharacter)) {
//			GetWorld()->DestroyActor(PlayerCharacter);
//		}
//
//		APlayerCharacter* NewPlayerCharacter = GetWorld()->SpawnActor<APlayerCharacter>(PlayerCharacterClass, GetPlayerStartTransform());
//		_PlayerController->Possess(NewPlayerCharacter);
//	}
//}



void AMainGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//AFPlayerState* PlayerState = NewPlayer->GetPlayerState<AFPlayerState>();
	//if (IsValid(PlayerState)) {
	//	PlayerState->InitPlayerState();
	//}
}

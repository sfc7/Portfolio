// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LobbyGameMode.h"
#include "Game/FPlayerState.h"
#include "Portfolio/Portfolio.h"
#include "Character/LobbyCharacter.h"
#include "Controller/UIController.h"

ALobbyGameMode::ALobbyGameMode()
{
	static ConstructorHelpers::FClassFinder<AUIController> UIControllerBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Controller/BP_LobbyController.BP_LobbyController_C'"));
	if (UIControllerBP.Succeeded())
	{
		PlayerControllerClass = UIControllerBP.Class;
	}

	static ConstructorHelpers::FClassFinder<ALobbyCharacter> LobbyCharacterBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Actor/Character/BP_LobbyP.BP_LobbyP_C'"));
	if (LobbyCharacterBP.Succeeded()) {
		LobbyCharacterClass = LobbyCharacterBP.Class;
	}

	PlayerStateClass = AFPlayerState::StaticClass();
}


//void ALobbyGameMode::SpawnPlayer(APlayerController* _PlayerController)
//{
//	Super::SpawnPlayer(_PlayerController);
//
//	ALobbyCharacter* LobbyCharacter = _PlayerController->GetPawn<ALobbyCharacter>();
//	if (IsValid(LobbyCharacter)) {
//		GetWorld()->DestroyActor(LobbyCharacter);
//	}
//
//	ALobbyCharacter* NewLobbyCharacter = GetWorld()->SpawnActor<ALobbyCharacter>(LobbyCharacterClass, GetPlayerStartTransform());
//	_PlayerController->Possess(NewLobbyCharacter);
//	
//}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

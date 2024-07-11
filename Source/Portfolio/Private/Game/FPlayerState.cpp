// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FPlayerState.h"
#include "Game/FGameInstance.h"
#include "Game/FGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Game/PlayerStateSave.h"
#include "Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

AFPlayerState::AFPlayerState()
{
	
}

void AFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AFPlayerState::InitPlayerState()
{

}

void AFPlayerState::SetPlayerNumber(int32 _PlayerNumber)
{
	if (HasAuthority()) {
		PlayerNumber = _PlayerNumber;
	}
}




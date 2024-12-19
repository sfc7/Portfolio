// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FPlayerState.h"
#include "Game/FGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Game/PlayerStateSave.h"
#include "Character/FCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Game/PlayerStateSave.h"
#include "Controller/PlayerCharacterController.h"

AFPlayerState::AFPlayerState()
{
	
}

void AFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AFPlayerState::InitPlayerState()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	AFCharacter* FCharacter = Cast<AFCharacter>(PlayerController->GetPawn());
	UPlayerStateSave* PlayerStateSave = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID),0));
	
	if (IsValid(PlayerStateSave)) {
		if (IsValid(FCharacter)) {
			FCharacter->SetPlayerMesh_Server(PlayerStateSave->PlayerMesh);
		}
	}
}

void AFPlayerState::SetPlayerNumber(int32 _PlayerNumber)
{
	if (HasAuthority()) {
		PlayerNumber = _PlayerNumber;
	}
}




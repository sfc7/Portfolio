// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Portfolio/Portfolio.h"


AFGameMode::AFGameMode()
{
	PlayerStartArray.SetNum(PlayerMaxCount);
}

void AFGameMode::PlayerStartFind()
{
	for (APlayerStart* Start : TActorRange<APlayerStart>(GetWorld())) {
		FString StartString = Start->PlayerStartTag.ToString();
		PlayerStartArray[UKismetStringLibrary::Conv_StringToInt(StartString)] = Start;
	}
}

FTransform AFGameMode::GetPlayerStartTransform()
{
	int32 PlayerNumber = FMath::Clamp(PlayerCount, 0, PlayerMaxCount - 1);
	PlayerCount++;

	return PlayerStartArray[PlayerNumber]->GetActorTransform();
}		

void AFGameMode::SpawnPlayer(APlayerController* _PlayerController)
{
}

void AFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerStartFind();
}

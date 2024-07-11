// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FGameState.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AFGameState : public AGameState
{
	GENERATED_BODY()
public:
	AFGameState();

	uint8 GetRoundNumber() const { return RoundNumber; }

	void SetTotalZombiesInRound (uint16 _TotalZombiesInRound);

	uint16 GetTotalZombiesInRound() const { return TotalZombiesInRound; }

	void IncrementRound() { ++RoundNumber; }

	void SpawnTotalZombiesInRound() { ++TotalZombiesInRound; }

	void ZombieDie() { --TotalZombiesInRound; UE_LOG(LogTemp, Log, TEXT("TotalZombiesInRound : %d"), TotalZombiesInRound);  }

private:
	uint8 RoundNumber;
	uint16 TotalZombiesInRound;




};

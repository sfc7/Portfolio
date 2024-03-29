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

private:
	uint8 RoundNumber;




};

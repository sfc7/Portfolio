// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FGameState.h"

AFGameState::AFGameState()
{
	CurrentRoundNumber = 1;
}

void AFGameState::SetTotalZombiesInRound(uint16 _TotalZombiesInRound)
{
	TotalZombiesInRound = _TotalZombiesInRound;
}

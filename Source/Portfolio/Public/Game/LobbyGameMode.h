// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/FGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ALobbyGameMode : public AFGameMode
{
	GENERATED_BODY()
public:
	ALobbyGameMode();

	//virtual void SpawnPlayer(APlayerController* _PlayerController) override;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	TSubclassOf<ALobbyCharacter> LobbyCharacterClass;
};
													
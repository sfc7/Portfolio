// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/FGameMode.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AMainGameMode : public AFGameMode
{
	GENERATED_BODY()
public:
	AMainGameMode();

	//virtual void SpawnPlayer(APlayerController* _PlayerController) override;

	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	TSubclassOf<class APlayerCharacter> PlayerCharacterClass;

};

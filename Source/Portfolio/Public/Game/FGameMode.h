// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FGameMode.generated.h"


/**
 * 
 */
UCLASS()
class PORTFOLIO_API AFGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AFGameMode();

	void PlayerStartFind();

	FTransform GetPlayerStartTransform();

	virtual void SpawnPlayer(APlayerController* _PlayerController);

protected:

	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	TArray<TObjectPtr<class APlayerStart>> PlayerStartArray;

	uint8 PlayerMaxCount;
	uint8 PlayerCount;


};

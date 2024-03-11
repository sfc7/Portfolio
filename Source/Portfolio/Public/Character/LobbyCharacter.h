// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FCharacter.h"
#include "LobbyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ALobbyCharacter : public AFCharacter
{
	GENERATED_BODY()

public:
	ALobbyCharacter();

	virtual void PossessedBy(AController* NewController) override;
protected:
	virtual void BeginPlay() override;
};

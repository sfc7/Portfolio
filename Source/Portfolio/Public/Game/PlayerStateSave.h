// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerStateSave.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayerStateSave : public USaveGame
{
	GENERATED_BODY()
public:
	UPlayerStateSave();

public:
	UPROPERTY()
		FString PlayerCharacterName;
	
	UPROPERTY()
		int32 CurrentLevel;

	UPROPERTY()
		float CurrentEXP;

	UPROPERTY()
		float Money;

	UPROPERTY()
		float CurrentAmmo;

	UPROPERTY()
		float TotalAmmo;

	UPROPERTY()
		class USkeletalMesh* PlayerMesh;



};

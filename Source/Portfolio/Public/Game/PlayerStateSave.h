// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Data/DataStruct.h"
#include "FGameInstance.h"
#include "PlayerStateSave.generated.h"


USTRUCT(BlueprintType)
struct FWeaponSlotSaveData
{
	GENERATED_USTRUCT_BODY()
		FWeaponSlotSaveData() {}

	UPROPERTY()
		FWeaponData FirstPrimaryWeaponData;

	UPROPERTY()
		FWeaponData SecondPrimaryWeaponData;

	UPROPERTY()
		uint8 Granade;
};
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
		int32 CurrentLevel;

	UPROPERTY()
		float Money;

	UPROPERTY()
		float CurrentEXP;

	UPROPERTY()
		FWeaponData FirstPrimaryWeaponData;

	UPROPERTY()
		FWeaponSlotSaveData WeaponSlotSaveData;

	UPROPERTY()
		class USkeletalMesh* PlayerMesh;



};

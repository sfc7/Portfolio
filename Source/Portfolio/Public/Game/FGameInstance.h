// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/AssetManager.h"
#include "Data/DataStruct.h"
#include "Character/FCharacter.h"
#include "FGameInstance.generated.h"

USTRUCT()
struct FWeaponSlotData
{
	GENERATED_USTRUCT_BODY()
		FWeaponSlotData() {}

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
class PORTFOLIO_API UFGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	virtual void Shutdown() override;

	UDataTable* GetCharacterTable() const  { return CharacterTable; }

	UDataTable* GetMonsterTable() const  { return MonsterTable; }

	FCharacterTable* GetCharacterTableRowFromLevel(int32 Level);

	FMonsterTable* GetMonsterTableRowFromName(FName _RowName);
	
	int32 GetTotalRoundNumber() const { return TotalRoundNumber; }

	void SetTotalRoundNumber(uint32 _Number) { TotalRoundNumber = _Number;  }

	void IncrementTotalRoundNumber() { TotalRoundNumber++; }

public:
	FStreamableManager StreamableManager = FStreamableManager();

	int32 CurrentAmmo = 0;
	int32 TotalAmmo = 0;
	int32 ReloadMaxAmmo = 0;
	uint8 bWeaponEquipFlag : 1;
	int32 CurrentLevel = 1;
	float CurrentEXP = 0;
	int32 PlayerMoney = 5000;

	UPROPERTY()
		FWeaponSlotData WeaponSlotData;

	UPROPERTY()
		TSubclassOf<class AWeapon> CurrentWeaponType;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateaccess = true))
		UDataTable* CharacterTable;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateaccess = true))
		UDataTable* MonsterTable;

	int32 TotalRoundNumber = 1;
};

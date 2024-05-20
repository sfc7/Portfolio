// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/AssetManager.h"
#include "FGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FCharacterTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	FCharacterTable() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxEXP;
};

USTRUCT(BlueprintType)
struct FMonsterTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	FMonsterTable() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EXP;
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

	FCharacterTable* UFGameInstance::GetCharacterTableRowFromLevel(int32 Level);

	FMonsterTable* UFGameInstance::GetMonsterTableRowFromName(FName _RowName);

public:
	FStreamableManager StreamableManager = FStreamableManager();

	uint32 CurrentAmmo = 0;
	uint32 TotalAmmo = 0;
	bool WeaponEquipFlag = false;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateaccess = true))
		class UDataTable* CharacterTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateaccess = true))
		class UDataTable* MonsterTable;
};

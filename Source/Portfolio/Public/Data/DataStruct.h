// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataStruct.generated.h"

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

UENUM()
enum class EItemType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Ammo UMETA(DisplayName = "Ammo"),
	Mundane UMETA(DisplayName = "Mundane")
};


USTRUCT(BlueprintType)
struct FPurchasableWeaponData : public FTableRowBase
{
	GENERATED_BODY()
public:
	FPurchasableWeaponData() :
		Name(FText::GetEmpty()),
		ItemType(EItemType::Mundane),
		CurrentAmmo(0),
		TotalAmmo(0),
		ReloadMaxAmmo(0),
		Price(0) {};

public:
	UPROPERTY(EditAnywhere)
		FText Name;

	UPROPERTY(EditAnywhere)
		EItemType ItemType;

	UPROPERTY(EditAnywhere)
		int32 CurrentAmmo;

	UPROPERTY(EditAnywhere)
		int32 TotalAmmo;

	UPROPERTY(EditAnywhere)
		int32 ReloadMaxAmmo;

	UPROPERTY(EditAnywhere)
		int32 Price;
};

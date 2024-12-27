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

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
	Consumable UMETA(DisplayName = "Consumable"),
	Mundane UMETA(DisplayName = "Mundane")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AR UMETA(DisplayName = "AR"),
	DMR UMETA(DisplayName = "DMR"),
	SG UMETA(DisplayName = "SG"),
	SMG UMETA(DisplayName = "SMG"),
	None UMETA(DisplayName = "None")
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()
public:
	FWeaponData() :
		Name(FText::GetEmpty()),
		ItemType(EItemType::Weapon),
		WeaponType(EWeaponType::None),
		CurrentAmmo(0),
		TotalAmmo(0),
		ReloadMaxAmmo(0),
		Price(0),
		Damage(0),
		RPM(0),
		Mesh(nullptr),
		ImpactEffect(nullptr),
		MuzzleFlash(nullptr)
	{};

public:
	UPROPERTY(EditAnywhere)
		FText Name;

	UPROPERTY(EditAnywhere)
		EItemType ItemType;

	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
		int32 CurrentAmmo;

	UPROPERTY(EditAnywhere)
		int32 TotalAmmo;

	UPROPERTY(EditAnywhere)
		int32 ReloadMaxAmmo;

	UPROPERTY(EditAnywhere)
		int32 Price;

	UPROPERTY(EditAnywhere)
		int32 Damage;

	UPROPERTY(EditAnywhere)
		int32 RPM;

	UPROPERTY(EditAnywhere)
		USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere)
		UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere)
		UParticleSystem* MuzzleFlash;
};

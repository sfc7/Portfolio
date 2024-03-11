// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ZombieCharacterSettings.generated.h"

/**
 * 
 */
UCLASS(config = ZombieCharacterMeshPaths)
class PORTFOLIOSETTINGS_API UZombieCharacterSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(config)
		TArray<FSoftObjectPath> ZombieCharacterMeshPaths;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerCharacterSettings.generated.h"

/**
 * 
 */
UCLASS(config = PlayerCharacterMuzzleLightMaterialPaths)
class PORTFOLIOSETTINGS_API UPlayerCharacterSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerCharacterMuzzleLightMaterialPaths;
	
};

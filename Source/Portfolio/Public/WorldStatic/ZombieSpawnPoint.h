// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "ZombieSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AZombieSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	AZombieSpawnPoint();

	bool IsUsed();

	void SetUsed(bool _bIsUsed);

protected:
	bool bIsUsed;




};

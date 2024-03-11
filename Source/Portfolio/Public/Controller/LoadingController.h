// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/UIController.h"
#include "LoadingController.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ALoadingController : public AUIController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
};

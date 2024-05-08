// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputConfig.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> ZoomAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> BurstTriggerAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> LandMineAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> MenuAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> Q_InteractionAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TObjectPtr<class UInputAction> ReloadAction;
		
};

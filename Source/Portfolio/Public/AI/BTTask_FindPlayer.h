// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UBTTask_FindPlayer : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_FindPlayer();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	float ClosestDistnace;

	AActor* FindPlayer;

	
};
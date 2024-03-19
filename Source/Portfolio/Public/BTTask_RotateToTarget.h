// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UBTTask_RotateToTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_TurnToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

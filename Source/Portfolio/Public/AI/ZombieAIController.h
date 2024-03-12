// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AZombieAIController : public AAIController
{
	GENERATED_BODY()
public:
	AZombieAIController();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void BeginAIController(APawn* InPawn);

	void EndAIController();

public:
	static const float PatrolRadius;

	static const FName StartPatrolPositionKey;

	static const FName EndPatrolPositionKey;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TObjectPtr<class UBlackboardData> BlackboardDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TObjectPtr<class UBehaviorTree> BehaviorTree;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GetEndPatrolPosition.h"
#include "AI/ZombieAIController.h"
#include "Character/ZombieCharacter.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetEndPatrolPosition::UBTTask_GetEndPatrolPosition()
{
	NodeName = TEXT("GetEndPatrolPosition");
}

EBTNodeResult::Type UBTTask_GetEndPatrolPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (EBTNodeResult::Failed == Result) {
		UE_LOG(LogTemp, Warning, TEXT("ExecuteTask"));
		return Result;
	}


	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (!IsValid(AIController)) {
		UE_LOG(LogTemp, Warning, TEXT("Controller"));
		return Result = EBTNodeResult::Failed;
	}

	AZombieCharacter* Zombie = Cast<AZombieCharacter>(AIController->GetPawn());
	if (!IsValid(Zombie)) {
		UE_LOG(LogTemp, Warning, TEXT("Zombie"));
		return Result = EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(Zombie->GetWorld());
	if (!IsValid(NavigationSystem)) {
		UE_LOG(LogTemp, Warning, TEXT("Navigation"));
		return Result = EBTNodeResult::Failed;
	}

	FVector StartPatrolPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AZombieAIController::StartPatrolPositionKey);
	FNavLocation EndPatrolLocation;
	if (NavigationSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, AIController->PatrolRadius, EndPatrolLocation)) {
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AZombieAIController::EndPatrolPositionKey, EndPatrolLocation.Location);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *EndPatrolLocation.Location.ToString());
		return Result = EBTNodeResult::Succeeded;
	}

	return Result;
}

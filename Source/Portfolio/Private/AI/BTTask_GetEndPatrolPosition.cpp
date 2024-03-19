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
		return Result;
	}

	UE_LOG(LogTemp, Log, TEXT("previous"));

	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController)) {
		
		AZombieCharacter* Zombie = Cast<AZombieCharacter>(AIController->GetPawn());
		if (IsValid(Zombie)) {
			UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(Zombie->GetWorld());

			if (IsValid(NavigationSystem)) {
				FVector StartPatrolPosition = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AZombieAIController::StartPatrolPositionKey);
				FNavLocation EndPatrolLocation;

				if (NavigationSystem->GetRandomPointInNavigableRadius(StartPatrolPosition, AIController->PatrolRadius, EndPatrolLocation)) {
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(AZombieAIController::EndPatrolPositionKey, EndPatrolLocation.Location);
					return Result = EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return Result = EBTNodeResult::Failed;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_Attack.h"
#include "AI/ZombieAIController.h"
#include "Character/ZombieCharacter.h"
#include "Component/MonsterComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController)) {
		AZombieCharacter* Zombie = Cast<AZombieCharacter>(AIController->GetPawn());

		if (IsValid(Zombie)) {
			if (!Zombie->GetMonsterComponent()->GetIsAttacking())
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	}
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController)) {
		AZombieCharacter* Zombie = Cast<AZombieCharacter>(AIController->GetPawn());

		if (IsValid(Zombie)) {
			Zombie->Attack();
		}
	}

	return EBTNodeResult::InProgress;
}

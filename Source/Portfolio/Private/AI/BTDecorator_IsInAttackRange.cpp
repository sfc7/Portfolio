// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_IsInAttackRange.h"
#include "AI/ZombieAIController.h"
#include "Character/ZombieCharacter.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("IsInAttackRange");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool Result = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	UE_LOG(LogTemp, Log, TEXT("decoprevious"));
	
	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController)) {
		AZombieCharacter* Zombie = Cast<AZombieCharacter>(AIController->GetPawn());

		if (IsValid(Zombie)) {
			APlayerCharacter* Target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AZombieAIController::TargetActorKey));
			if (IsValid(Target)) {
				UE_LOG(LogTemp, Log, TEXT("decosuccess"));
				return Result = Target->GetDistanceTo(Zombie) <= AttackRange;
			}
		}
	}

	return Result = false;	
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_SetMovementSpeed.h"
#include "AI/ZombieAIController.h"
#include "Character/ZombieCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed()
{
	NodeName = TEXT("SetMovementSpeed");
}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (EBTNodeResult::Failed == Result) {
		return Result;
	}

	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController)) {
		AZombieCharacter* ZombieChaacter = Cast<AZombieCharacter>(AIController->GetPawn());

		if (IsValid(ZombieChaacter)) {
			ZombieChaacter->GetCharacterMovement()->MaxWalkSpeed = Speed;
			Result = EBTNodeResult::Succeeded;
		}
	}

	return Result = EBTNodeResult::Failed;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RotateToTarget.h"
#include "AI/ZombieAIController.h"
#include "Character/ZombieCharacter.h"
#include "Character/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RotateToTarget::UBTTask_RotateToTarget()
{
	NodeName = TEXT("RotateToTarget");
}

EBTNodeResult::Type UBTTask_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController)) {
		AZombieCharacter* Zombie = Cast<AZombieCharacter>(AIController->GetPawn());

		if (IsValid(Zombie)) {
			APlayerCharacter* Target = Cast<APlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AZombieAIController::TargetActorKey));
			if (IsValid(Target)) {
				FVector VectorToTarget = Target->GetActorLocation() - Zombie->GetActorLocation();
				VectorToTarget.Z = 0.f;
				FRotator TargetRotation = FRotationMatrix::MakeFromX(VectorToTarget).Rotator();
				Zombie->SetActorRotation(FMath::RInterpTo(Zombie->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 2.f));

				return Result = EBTNodeResult::Succeeded;
			}
		}
	}
	
	return Result = EBTNodeResult::Failed;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ZombieAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

const float AZombieAIController::PatrolRadius(100.f);
const FName AZombieAIController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName AZombieAIController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));

AZombieAIController::AZombieAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();

	APawn* OwnerPawn = GetPawn();
	if (IsValid(OwnerPawn)) {
		BeginAIController(OwnerPawn);
	}
}

void AZombieAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	EndAIController();
}

void AZombieAIController::BeginAIController(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent)) {
		if (UseBlackboard(BlackboardDataAsset, BlackboardComponent)) {
			RunBehaviorTree(BehaviorTree);
			BlackboardComponent->SetValueAsVector(StartPatrolPositionKey, InPawn->GetActorLocation());
		}
	}
}

void AZombieAIController::EndAIController()
{
	UBehaviorTreeComponent* BehaivorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaivorTreeComponent)) {
		BehaivorTreeComponent->StopTree();
	}
}


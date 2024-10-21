// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ZombieAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Character/PlayerCharacter.h"
#include "Character/ZombieCharacter.h"
#include "Game/MainGameMode.h"
#include "Controller/PlayerCharacterController.h"

const float AZombieAIController::PatrolRadius(100.f);
const FName AZombieAIController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName AZombieAIController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));
const FName AZombieAIController::TargetActorKey(TEXT("TargetActor"));
const FName AZombieAIController::IsAttackingKey(TEXT("IsAttacking"));

AZombieAIController::AZombieAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));

	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception")));

	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 50000.f;
	SightConfig->LoseSightRadius = 50000.f;
	SightConfig->PeripheralVisionAngleDegrees = 360.f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;
	SightConfig->PointOfViewBackwardOffset = 0.0f;
	SightConfig->NearClippingRadius = 100.f;
	SightConfig->SetMaxAge(0.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AZombieCharacter* OwnerPawn = Cast<AZombieCharacter>(GetPawn());
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
			BlackboardComponent->SetValueAsVector(AZombieAIController::StartPatrolPositionKey, InPawn->GetActorLocation());
		}
	}

	AMainGameMode* MainGameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(MainGameMode)) {
		float ClosestDistance = FLT_MAX;
		AActor* ClosestActor = nullptr;

		for (APlayerCharacterController* PlayerCharacterController : MainGameMode->AlivePlayerCharacterControllers) {
			float Distance = PlayerCharacterController->GetPawn()->GetDistanceTo(GetPawn());
			if (ClosestDistance > Distance) {
				ClosestDistance = Distance;
				ClosestActor = PlayerCharacterController->GetPawn();
			}
		}

		if (IsValid(ClosestActor)) {
			Blackboard->SetValueAsObject(ThisClass::TargetActorKey, ClosestActor);
		}
	}
}

void AZombieAIController::SetTargetActorKeyInBlackboard(AActor* Target)
{
	if (IsValid(Blackboard)) {
		Blackboard->SetValueAsObject(AZombieAIController::TargetActorKey, Target);
	}
}

void AZombieAIController::SetIsAttackingInBlackboard(bool _IsAttacking)
{
	if (IsValid(Blackboard)) {
		Blackboard->SetValueAsBool(AZombieAIController::IsAttackingKey, _IsAttacking);
	}
}

void AZombieAIController::EndAIController()
{
	UBehaviorTreeComponent* BehaivorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaivorTreeComponent)) {
		BehaivorTreeComponent->StopTree();
	}
}

void AZombieAIController::SetTargetKeybySightSense(AActor* actor, FAIStimulus const Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass())) {
		if (APlayerCharacter* UpdateTarget = Cast<APlayerCharacter>(actor)) {
			if (Stimulus.WasSuccessfullySensed()) {
				if (!IsValid(CurrentTarget)) {
					CurrentTarget = UpdateTarget;
					Blackboard->SetValueAsObject(AZombieAIController::TargetActorKey, actor);
				}
				else {
					if (CurrentTarget != UpdateTarget) {
						if (CurrentTarget->GetDistanceTo(GetPawn()) > UpdateTarget->GetDistanceTo(GetPawn())) {
							CurrentTarget = UpdateTarget;
							Blackboard->SetValueAsObject(AZombieAIController::TargetActorKey, actor);
						}
					}
				}
			}
		}
	}
}


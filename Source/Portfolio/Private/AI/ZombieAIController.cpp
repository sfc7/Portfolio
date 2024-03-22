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

const float AZombieAIController::PatrolRadius(100.f);
const FName AZombieAIController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName AZombieAIController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));
const FName AZombieAIController::TargetActorKey(TEXT("TargetActor"));

AZombieAIController::AZombieAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));

	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception")));

	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 500.f;
	SightConfig->LoseSightRadius = 500.f;
	SightConfig->PeripheralVisionAngleDegrees = 360.f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;
	SightConfig->PointOfViewBackwardOffset = 0.0f;
	SightConfig->NearClippingRadius = 100.f;
	SightConfig->SetMaxAge(0.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieAIController::SetTargetKeybySightSense);
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

void AZombieAIController::SetTargetKeybySightSense(AActor* actor, FAIStimulus const Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass())) {
		if (Cast<APlayerCharacter>(actor)) {
			if (Stimulus.WasSuccessfullySensed()) {
				Blackboard->SetValueAsObject(AZombieAIController::TargetActorKey, actor);
			}
		}
	}
}


	// Fill out your copyright notice in the Description page of Project Settings.


	#include "AI/BTService_DetectPlayerCharacter.h"
	#include "Character/ZombieCharacter.h"
	#include "Character/PlayerCharacter.h"
	#include "AI/ZombieAIController.h"
	#include "Controller/PlayerCharacterController.h"
	#include "Game/MainGameMode.h"
	#include "BehaviorTree/BlackboardData.h"
	#include "BehaviorTree/BlackboardComponent.h"
	#include "BehaviorTree/BlackboardComponent.h"
	#include "Kismet/GameplayStatics.h"

	UBTService_DetectPlayerCharacter::UBTService_DetectPlayerCharacter()
	{
		NodeName = TEXT("DetectPlayerCharacter");
		Interval = 2.f;
	}

	void UBTService_DetectPlayerCharacter::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
	{
		Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


		AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
		if (IsValid(AIController)) {
			AMainGameMode* MainGameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
			float ClosestDistance = FLT_MAX;
			AActor* ClosestActor = nullptr;

			TArray<AActor*> PlayerCharacters;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), PlayerCharacters);

			for (AActor* PlayerCharacter : PlayerCharacters) {
				float Distance = PlayerCharacter->GetDistanceTo(OwnerComp.GetOwner());
				if (ClosestDistance > Distance) {
					ClosestDistance = Distance;
					ClosestActor = PlayerCharacter;
				}
			}


			UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
			BlackboardComponent->SetValueAsObject(AZombieAIController::TargetActorKey, ClosestActor);
		}
	}
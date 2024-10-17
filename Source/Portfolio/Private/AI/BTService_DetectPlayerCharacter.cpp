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
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

UBTService_DetectPlayerCharacter::UBTService_DetectPlayerCharacter()
{
	NodeName = TEXT("DetectPlayerCharacter");
	Interval = 1.f;
}

void UBTService_DetectPlayerCharacter::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController)) {
		UAIPerceptionComponent* PerceptionComponent = AIController->FindComponentByClass<UAIPerceptionComponent>();

		TArray<AActor*> PerceivedActorArray;
		PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActorArray);

		if (PerceivedActorArray.Num() > 0)
		{
			UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
			if (IsValid(BlackboardComponent))
			{
				AActor* ClosestActor = nullptr;
				float ClosestDistance = FLT_MAX;

				for (AActor* Actor : PerceivedActorArray)
				{
					APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
					if (IsValid(PlayerCharacter))
					{
						float Distance = AIController->GetPawn()->GetDistanceTo(PlayerCharacter);
						if (ClosestDistance > Distance)
						{
							ClosestDistance = Distance;
							ClosestActor = PlayerCharacter;
						}
					}
				}

				BlackboardComponent->SetValueAsObject(AZombieAIController::TargetActorKey, ClosestActor);
			}
		}
	}
	
}
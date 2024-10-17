// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_FindPlayer.h"
#include "Character/ZombieCharacter.h"
#include "Character/PlayerCharacter.h"
#include "AI/ZombieAIController.h"
#include "Controller/PlayerCharacterController.h"
#include "Game/MainGameMode.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPlayer::UBTTask_FindPlayer()
{
	ClosestDistnace = FLT_MAX;
}

EBTNodeResult::Type UBTTask_FindPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);


	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController)) {
		AZombieCharacter* Zombie = Cast<AZombieCharacter>(AIController->GetPawn());

		if (IsValid(Zombie)) {
			AMainGameMode* MainGameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
			
			for (int32 i = 0; i < MainGameMode->AlivePlayerCharacterControllers.Num(); ++i) {
				APlayerCharacterController* PlayerCharacterController = MainGameMode->AlivePlayerCharacterControllers[i];
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerCharacterController->GetPawn());
				
				if (IsValid(PlayerCharacter)) {
					if (ClosestDistnace > PlayerCharacter->GetDistanceTo(Zombie)) {
						ClosestDistnace = PlayerCharacter->GetDistanceTo(Zombie);
						FindPlayer = PlayerCharacter;
					}
				}
			}

			if (FindPlayer == nullptr) {
				AIController->SetTargetActorKeyInBlackboard(nullptr);
			}
			else {
				AIController->SetTargetActorKeyInBlackboard(FindPlayer);
			}
		}
	}




	return EBTNodeResult::Type();
}

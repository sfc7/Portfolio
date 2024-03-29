// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/FGameMode.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AMainGameMode : public AFGameMode
{
	GENERATED_BODY()
public:
	AMainGameMode();

	void SpawnZombie();	

	//virtual void SpawnPlayer(APlayerController* _PlayerController) override;
	
protected:

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	void CountZombieRemaning();

private:
	TObjectPtr<class AFGameState> FGameState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class AZombieCharacter> ZombieCharacterClass;

	FTimerHandle ZombieSpawnHandle;
		
	TArray<TObjectPtr<class AZombieSpawnPoint>> ZombieSpawnPointArray;

	uint16 ZombieRemaning;





	TSubclassOf<class APlayerCharacter> PlayerCharacterClass;

};

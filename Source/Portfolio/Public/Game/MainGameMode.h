// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/FGameMode.h"
#include "MainGameMode.generated.h"

UENUM(BlueprintType)
enum class ELevelState : uint8
{
	None,
	WaitingRoom,
	Stage,
	End
};

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

	void RemaningZombieDie();

	//virtual void SpawnPlayer(APlayerController* _PlayerController) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	void SetZombieRemaning();

	UFUNCTION()
	void OnMainTimerElapsed();

	void OnNotificationText(const FString& NotificationString);

	void SetLevelStateFromLevelName();

public:

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<TObjectPtr<class APlayerCharacterController>> AlivePlayerCharacterControllers;

private:
	TObjectPtr<class AFGameState> FGameState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class AZombieCharacter> ZombieCharacterClass;

	FTimerHandle ZombieSpawnHandle;
		
	TArray<TObjectPtr<class AZombieSpawnPoint>> ZombieSpawnPointArray;

	uint16 ZombieSpawnRemaning = 0 ;

	FTimerHandle MainTimerHandle;

	ELevelState LevelState = ELevelState::WaitingRoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		int32 WaitingRoomTime = 1000;

	int32 RemaningWaitTime = 1000;

	TSubclassOf<class APlayerCharacter> PlayerCharacterClass;

	FString NotificationString;

	FString CurrentLevelName;

};

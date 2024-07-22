// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Game/FGameMode.h"
#include "MainGameMode.generated.h"

USTRUCT(BlueprintType)
struct FZombieRoundTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	FZombieRoundTable() {}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Round;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Number;
};

UENUM(BlueprintType)
enum class ELevelState : uint8
{
	None,
	Room,
	WaitingStage,
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

	void ZombieDie();

	void EndMap();

	void InRoom();

	void WaitStage();

	void InStage();

	UDataTable* GetZombieRoundTable() const { return ZombieRoundTable; }

	FZombieRoundTable* GetZombieRoundTableRowFromRound(int _Round);

protected:
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

private:
	void SetZombieRemaning();

	UFUNCTION()
	void OnMainTimerElapsed();

	void OnNotificationText(const FString& NotificationString);

	void SetLevelStateFromLevelName();

	void SetLevelStateFromString(const FString& _LevelState);
public:
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<TObjectPtr<class APlayerCharacterController>> PlayerCharacterControllers;

private:
	TObjectPtr<class AFGameState> FGameState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateaccess = true))
		class UDataTable* ZombieRoundTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
		TSubclassOf<class AZombieCharacter> ZombieCharacterClass;

	FTimerHandle ZombieSpawnHandle;
		
	TArray<TObjectPtr<class AZombieSpawnPoint>> ZombieSpawnPointArray;	

	uint32 CurrentRound = 1;

	uint16 ZombieSpawnRemaning = 0;

	FTimerHandle MainTimerHandle;

	ELevelState LevelState = ELevelState::Room;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		int32 RoomTime = 40;

	int32 RemaningWaitTime = 40;

	TSubclassOf<class APlayerCharacter> PlayerCharacterClass;

	FString NotificationString;

	FString CurrentLevelName;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentLevelChangedDelegate, int32, _CurrentLevel, int32, NewCurrentLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentEXPChangedDelegate, float, _CurrentEXP, float, NewCurrentEXP);

UCLASS()
class PORTFOLIO_API AFPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
    AFPlayerState();

    void InitPlayerState();

    int32 GetPlayerNumber() const { return PlayerNumber; }

    int32 GetMaxLevel() const { return MaxLevel; }

    int32 GetCurrentLevel() const { return CurrentLevel; }

    float GetMaxEXP() const { return MaxEXP; }

    float GetCurrentEXP() const { return CurrentEXP; }

    void SetCurrentLevel(int32 _CurrentLevel);

    void SetCurrentEXP(float _CurrentEXP);

    void SetPlayerNumber(int32 _PlayerNumber);

    
public:
    FOnCurrentLevelChangedDelegate OnCurrentLevelChangedDelegate;

    FOnCurrentEXPChangedDelegate OnCurrentEXPChangedDelegate;


private:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        TObjectPtr<class UFGameInstance> FGameInstance;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 PlayerNumber = 99;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 MaxLevel = 5;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 CurrentLevel = 1;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        float MaxEXP = 150;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        float CurrentEXP = 0;

};

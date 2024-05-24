// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPlayerState.generated.h"

// 게임의 세션이 이동해도 저장될 값들

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentLevelChangedDelegate, int32, NewCurrentLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentEXPChangedDelegate, float, NewCurrentEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxEXPChangedDelegate, float, NewMaxEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChangeDelegate, int32, NewMoney);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentAmmoAndTotalAmmoChangeDelegate, int32, CurrentAmmo, int32, TotalAmmo);


UCLASS()
class PORTFOLIO_API AFPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
    AFPlayerState();

    void InitPlayerState();

    void SaveFGameInstance(); 

    void SaveInitValueFromPlayerStateSave();

    int32 GetPlayerNumber() const { return PlayerNumber; }

    int32 GetMaxLevel() const { return MaxLevel; }

    int32 GetCurrentLevel() const { return CurrentLevel; }

    float GetMaxEXP() const { return MaxEXP; }

    float GetCurrentEXP() const { return CurrentEXP; }

    int32 GetMoney() const { return PlayerMoney; }

    int32 GetReloadMaxAmmo() const { return ReloadMaxAmmo; }

    int32 GetCurrentAmmo() const { return CurrentAmmo; }

    int32 GetTotalAmmo() const { return TotalAmmo; }

    bool GetWeaponEquipFlag();

    void SetCurrentLevel(int32 _CurrentLevel);

    void SetCurrentEXP(float _CurrentEXP);

    void SetPlayerNumber(int32 _PlayerNumber);

    void SetMoney(int32 _Money);

    void SetReloadMaxAmmo(int32 _ReloadMaxAmmo);

    void SetTotalAmmo(int32 _TotalAmmo);

    void SetCurrentAmmo(int32 _CurrentAmmo);

    void SetCurrentAndTotalAmmo(int32 _CurrentAmmo, int32 _TotalAmmo);

    void SetWeaponEquipFlagOn();

    virtual void OnDeactivated() override;


public:
    FOnCurrentLevelChangedDelegate OnCurrentLevelChangedDelegate;

    FOnCurrentEXPChangedDelegate OnCurrentEXPChangedDelegate;

    FOnMaxEXPChangedDelegate OnMaxEXPChangedDelegate;

    FOnMoneyChangeDelegate OnMoneyChangeDelegate;   

    FOnCurrentAmmoAndTotalAmmoChangeDelegate OnCurrentAmmoAndTotalAmmoChangeDelegate;

    bool PlayerStateSaveFlag = false;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        TObjectPtr<class UFGameInstance> FGameInstance;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 PlayerNumber = 99;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 MaxLevel;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 CurrentLevel;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        float MaxEXP;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        float CurrentEXP;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 PlayerMoney;

    UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 ReloadMaxAmmo;

    UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 TotalAmmo;

    UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 CurrentAmmo;



};

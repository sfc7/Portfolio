// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPlayerState.generated.h"



UCLASS()
class PORTFOLIO_API AFPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
    AFPlayerState();

    void InitPlayerState();

    int32 GetPlayerNumber() const { return PlayerNumber; }

    void SetPlayerNumber(int32 _PlayerNumber);


protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        TObjectPtr<class UFGameInstance> FGameInstance;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
        int32 PlayerNumber = 99;
};

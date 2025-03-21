// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UMonsterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMonsterComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float GetCurrentHp() const { return CurrentHp; }

	float GetMaxHp() const { return MaxHp; }

	float GetMonsterExpValue() const { return MonsterExpValue; }

	uint8 GetIsAttacking() const { return bIsAttacking; }

	uint8 GetIsDead() const { return bIsDead; }

	void SetMonsterName(FName _Name);

	void SetCurrentHp(float _CurrentHp);

	void SetMaxHp(float _MaxHp);

	void SetMonsterExpValue(float _MonsterExpValue);

	void SetIsAttacking(uint8 _bIsAttacking);

	void SetIsDead(uint8 _bIsDead);

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UFGameInstance> GameInstance;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float CurrentHp;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float MaxHp;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		FName MonsterName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float MonsterExpValue;

	UPROPERTY(Replicated, meta = (AllowPrivateAccess))
		uint8 bIsAttacking : 1;

	UPROPERTY(Replicated, meta = (AllowPrivateAccess))
		uint8 bIsDead : 1;


		
};

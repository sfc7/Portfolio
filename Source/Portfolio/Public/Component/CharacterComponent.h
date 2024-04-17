// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterComponent.generated.h"

// 각 게임의 세션 안에서만 유효한 값들

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrnetHpChangeDelegate, float, _CurrentHp, float, NewCurrentHp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHpChangeDelegate, float, _MaxHp, float, NewMaxHp);

UENUM(BlueprintType)
enum class ECurrentState : uint8
{
	None,
	Stand,
	Crouch,
	End
};

UCLASS(ClassGroup = (Custom),  meta = (BlueprintSpawnableComponent))
class PORTFOLIO_API UCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterComponent();

	friend class APlayerCharacter;

	float GetCurrentHp() const { return CurrentHp; }

	float GetMaxHp() const { return MaxHp; }

	void SetCurrentHp(float _CurrentHp);

	void SetMaxHp(float _MaxHp);

	void EquipWeapon(class AWeapon* _Weapon);

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	void SetAiming(bool _bIsAming);

	UFUNCTION(Server, Reliable)
		void SetAiming_Server(bool _bIsAiming);

	void SetCurrenState(ECurrentState _CurrentState);

	UFUNCTION(Server, Reliable)
		void SetCurrenState_Server(ECurrentState _CurrentState);

private:
	UFUNCTION(NetMulticast, Reliable)
		void OnCurrentHPChanged_NetMulticast(float _CurrentHp, float NewCurrntHp);
	UFUNCTION()
		void OnCurrentLevelChanged(int32 NewCurrentLevel);

public:
	FOnCurrnetHpChangeDelegate OnCurrnetHpChangeDelegate;

	FOnMaxHpChangeDelegate OnMaxHpChangeDelegate;

	 
	static FString SaveSlotName;
		
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UFGameInstance> GameInstance;

	class APlayerCharacter* PlayerCharacter;

	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float CurrentHp;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float MaxHp;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float CurrentExp;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float MaxExp;

	UPROPERTY(Replicated)
		ECurrentState CurrentState = ECurrentState::Stand;

	UPROPERTY(Replicated)
		uint8 bIsAiming : 1;

	UPROPERTY(Replicated)
		uint8 bIsDead : 1; 


		
};

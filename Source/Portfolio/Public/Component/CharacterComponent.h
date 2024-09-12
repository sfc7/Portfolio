// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterComponent.generated.h"

// 각 게임의 세션 안에서만 유효한 값들

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrnetHpChangeDelegate, float, _CurrentHp, float, NewCurrentHp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHpChangeDelegate, float, _MaxHp, float, NewMaxHp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentAmmoAndTotalAmmoChangeDelegate, int32, CurrentAmmo, int32, TotalAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentLevelChangedDelegate, int32, NewCurrentLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentEXPChangedDelegate, float, NewCurrentEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxEXPChangedDelegate, float, NewMaxEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChangeDelegate, int32, NewMoney);


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

	int32 GetReloadMaxAmmo() const { return ReloadMaxAmmo; }

	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	int32 GetTotalAmmo() const { return TotalAmmo; }

	TSubclassOf<class AWeapon> GetCurrentWeaponType() const { return CurrentWeaponType; }

	TSubclassOf<class AWeapon> GetDefaultWeaponType() const { return DefaultWeaponType; }

	void SetReloadMaxAmmo(int32 _ReloadMaxAmmo);

	void SetTotalAmmo(int32 _TotalAmmo);

	void SetCurrentAmmo(int32 _CurrentAmmo);

	void SetCurrentAndTotalAmmo(int32 _CurrentAmmo, int32 _TotalAmmo);

	void SetCurrentWeaponType(TSubclassOf<class AWeapon> _CurrentWeaponType);

	void SetDefaultWeaponType(TSubclassOf<class AWeapon> _DefaultWeaponType);

	int32 GetMaxLevel() const { return MaxLevel; }

	int32 GetCurrentLevel() const { return CurrentLevel; }

	float GetMaxEXP() const { return MaxEXP; }

	float GetCurrentEXP() const { return CurrentEXP; }

	int32 GetMoney() const { return PlayerMoney; }

	void SetCurrentLevel(int32 _CurrentLevel);

	void SetCurrentEXP(float _CurrentEXP);
	
	void SetMoney(int32 _Money);

	UFUNCTION(Server, Reliable)
		void SendGameInstanceWeaponVariable_Server(int32 _TotalAmmo, int32 _CurrentAmmo, int32 _ReloadMaxAmmo, TSubclassOf<AWeapon> _CurrentWeaponType);

	UFUNCTION(Server, Reliable)
		void SendGameInstanceXPVariable_Server(int32 _CurrentLevel, int32 _CurrentEXP, int32 _Money);
	
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

	UFUNCTION()
		void OnRep_CurrentWeapon(); 

	UFUNCTION()
		void OnRep_TotalWeapon();

	UFUNCTION()
		virtual void OnRep_CurrentEXP();

	UFUNCTION()
		virtual void OnRep_MaxEXP();

	UFUNCTION()
		virtual void OnRep_PlayerMoney();

	UFUNCTION()
		virtual void OnRep_CurrentLevel();

public:
	FOnCurrnetHpChangeDelegate OnCurrnetHpChangeDelegate;

	FOnMaxHpChangeDelegate OnMaxHpChangeDelegate;

	FOnCurrentAmmoAndTotalAmmoChangeDelegate OnCurrentAmmoAndTotalAmmoChangeDelegate;

	FOnCurrentLevelChangedDelegate OnCurrentLevelChangedDelegate;

	FOnCurrentEXPChangedDelegate OnCurrentEXPChangedDelegate;

	FOnMaxEXPChangedDelegate OnMaxEXPChangedDelegate;

	FOnMoneyChangeDelegate OnMoneyChangeDelegate;

		
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UFGameInstance> FGameInstance;

	class APlayerCharacter* PlayerCharacter;

	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float CurrentHp;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float MaxHp;


	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		ECurrentState CurrentState = ECurrentState::Stand;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		uint8 bIsAiming : 1;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		uint8 bIsDead : 1; 

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		int32 ReloadMaxAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_TotalWeapon, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		int32 TotalAmmo;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		int32 CurrentAmmo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TSubclassOf<AWeapon> DefaultWeaponType;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TSubclassOf<AWeapon> CurrentWeaponType;

	UPROPERTY(Replicated)
		int32 MaxLevel;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentLevel, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		int32 CurrentLevel;

	UPROPERTY(ReplicatedUsing = OnRep_MaxEXP, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float MaxEXP;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentEXP, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float CurrentEXP;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerMoney, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		int32 PlayerMoney;


};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/AimingHUD.h"
#include "CharacterComponent.generated.h"

// 각 게임의 세션 안에서만 유효한 값들

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrnetHpChangeDelegate, int, _CurrentHp, int, NewCurrentHp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHpChangeDelegate, int, _MaxHp, int, NewMaxHp);
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
	Grenade,
	End
};

UCLASS(ClassGroup = (Custom),  meta = (BlueprintSpawnableComponent))
class PORTFOLIO_API UCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterComponent();

	friend class APlayerCharacter;

	int32 GetCurrentHp() const { return CurrentHp; }

	int32 GetMaxHp() const { return MaxHp; }

	void SetCurrentHp(int32 _CurrentHp);

	void SetMaxHp(int32 _MaxHp);

	void EquipWeapon(class AWeapon* _Weapon);

	int32 GetReloadMaxAmmo() const { return ReloadMaxAmmo; }

	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	int32 GetTotalAmmo() const { return TotalAmmo; }

	TSubclassOf<class AWeapon> GetCurrentWeaponType() const { return CurrentWeaponType; }

	TSubclassOf<class AWeapon> GetDefaultFirstPrimaryWeaponType() const { return DefaultFirstPrimaryWeaponType; }

	TSubclassOf<class AWeapon> GetDefaultSecondPrimaryWeaponType() const { return DefaultSecondPrimaryWeaponType; }

	void SetReloadMaxAmmo(int32 _ReloadMaxAmmo);

	void SetTotalAmmo(int32 _TotalAmmo);

	void SetCurrentAmmo(int32 _CurrentAmmo);

	void SetCurrentAndTotalAmmo(int32 _CurrentAmmo, int32 _TotalAmmo);

	void SetCurrentWeaponType(TSubclassOf<class AWeapon> _CurrentWeaponType);

	void SetDefaultFirstPrimaryWeaponType(TSubclassOf<class AWeapon> _DefaultFirstPrimaryWeaponType);

	void SetDefaultSecondPrimaryWeaponType(TSubclassOf<class AWeapon> _DefaultSecondPrimaryWeaponType);

	int32 GetMaxLevel() const { return MaxLevel; }

	int32 GetCurrentLevel() const { return CurrentLevel; }

	float GetMaxEXP() const { return MaxEXP; }

	float GetCurrentEXP() const { return CurrentEXP; }

	int32 GetMoney() const { return PlayerMoney; }

	void SetCurrentLevel(int32 _CurrentLevel);

	void SetCurrentEXP(float _CurrentEXP);
	
	void SetMoney(int32 _Money);

	uint8 GetIsDead() const { return bIsDead; }

	void SetIsDead(uint8 _bIsDead);

	UFUNCTION(Server, Reliable)
		void SendGameInstanceWeaponVariable_Server(int32 _TotalAmmo, int32 _CurrentAmmo, int32 _ReloadMaxAmmo, TSubclassOf<AWeapon> _CurrentWeaponType);

	UFUNCTION(Server, Reliable)
		void SendGameInstanceXPVariable_Server(int32 _CurrentLevel, int32 _CurrentEXP, int32 _Money);

	void SetCrossHairColor_Red();
	
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

	void SetHUDCrossHair(float DeltaTime);

private:
	UFUNCTION(NetMulticast, Reliable)
		void OnCurrentHPChanged_NetMulticast(int32 _CurrentHp, int32 NewCurrntHp);

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

	FOnCurrentLevelChangedDelegate OnCurrentLevelChangedDelegate;

	FOnCurrentEXPChangedDelegate OnCurrentEXPChangedDelegate;

	FOnMaxEXPChangedDelegate OnMaxEXPChangedDelegate;

	FOnMoneyChangeDelegate OnMoneyChangeDelegate;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		class UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		UTexture2D* CrosshairTop;

	UPROPERTY(EditAnywhere, Category = Crosshair)
		UTexture2D* CrosshairBottom;

		
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UFGameInstance> FGameInstance;

	class APlayerCharacterController* PlayerCharacterController;

	class AAimingHUD* AimingHUD;

	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		AWeapon* EquippedWeapon;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		int32 CurrentHp;

	UPROPERTY(Replicated, VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		int32 MaxHp;


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
		TSubclassOf<AWeapon> DefaultFirstPrimaryWeaponType;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TSubclassOf<AWeapon> DefaultSecondPrimaryWeaponType;

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

	FCrossHairTexture CrossHairTexture;

	float CrossHairVelocityValue;

	float CrossHairJumpValue;

	float CrossHairAimingValue;

	float CrossHairShootingValue;
};

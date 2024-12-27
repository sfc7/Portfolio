// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FCharacter.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Component/CharacterComponent.h"
#include "Interface/InteractionInterface.h"
#include "Data/DataStruct.h"
#include "Components/TimeLineComponent.h" 
#include "Engine/DataTable.h"
#include "PlayerCharacter.generated.h"


USTRUCT()
struct FInteractionData {
	GENERATED_USTRUCT_BODY()

	FInteractionData() : 
		CurrentInteractable(nullptr), 
		LastInteractionCheckTime(0.0f) 
	{};

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

UCLASS()
class PORTFOLIO_API APlayerCharacter : public AFCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	float GetForwardInputValue() const { return ForwardInputValue; }

	float GetRightInputValue() const { return RightInputValue; }

	float GetAimPitch() const { return AimPitch; }

	float GetAimYaw() const { return AimYaw; }

	bool IsAiming();

	bool IsDead();

	ECurrentState IsCurrentState();

	bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(InteractionTimerHandle); }

	UPROPERTY(Replicated)
	bool bThrowMontageEndFlag;

	UPROPERTY(EditAnywhere)
		TObjectPtr<class USplineComponent> SplinePath;

	UPROPERTY(EditAnywhere)
		class UStaticMesh* SplineStaticMesh;

	UPROPERTY(EditAnywhere)
		class UMaterialInterface* SplineStaticMaterial;


	TArray<class USplineMeshComponent*> SplineMeshComponents;


protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	virtual void OnRep_PlayerState() override;

private:
	void Move(const FInputActionValue& InValue);

	void Look(const FInputActionValue& InValue);

	void Attack(const FInputActionValue& InValue);

	void Crouch(const FInputActionValue& InValue);

	void StartAiming(const FInputActionValue& InValue);
		 
	void EndAiming(const FInputActionValue& InValue);

	void ToggleBurstTrigger(const FInputActionValue& InValue);

	void AttackOnBurstTrigger(const FInputActionValue& InValue);

	void StopFire(const FInputActionValue& InValue);

	void OnMenu(const FInputActionValue& InValue);

	void Fire();

	void FireBullet();

	void FireBullet_LineTrace(FVector _CameraStartLocation, FVector _CameraEndLocation);

	FRotator GetSprayAngle(FVector _MuzzleDirection, float _MaxAngle);

	void Reload();

	void ReloadAmmo();

	void ThrowGrenade(const FInputActionValue& InValue);

	void ThrowGrenadeDistance(const FInputActionValue& InValue);

	void UpdateDestroyedActor();

	UFUNCTION()
		void StartHorizontalRecoil(float Value);

	UFUNCTION()
		void StartVerticalRecoil(float Value);

	void StartRecoil();

	void ReverseRecoil();

	// Attack
	UFUNCTION(Server, Unreliable)
		void PlayAttackMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
		void PlayAttackMontage_NetMulticast();

	UFUNCTION(Server, Reliable)
		void ApplyDamageAndDrawLine_Server(ACharacter* HitCharacter, const FHitResult& HitResult, float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
		void DrawLine_NetMulticast(const FVector& DrawStart, const FVector& DrawEnd);

	UFUNCTION(Server, Reliable)
		void UseAmmo_Server();

		void UseAmmo();

	void FireAnimationPlay();
	//

	UFUNCTION(Server, Unreliable)
		void PlayHitMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
		void PlayHitMontage_NetMulticast();

	void HitAnimationPlay();

	UFUNCTION()
		void OnCurrentLevelChanged(int32 NewCurrentLevel);	

	UFUNCTION(Server, Unreliable)
		void UpdateInputValue_Server(const float& _ForwardInputValue, const float& _RightInputValue);

	UFUNCTION(Server, Unreliable)
		void UpdateAimValue_Server(const float& _AimPitch, const float& _AimYaw);

	UFUNCTION(NetMulticast, Unreliable)
		void UpdateAimValue_NetMulticast(const float& _AimPitch, const float& _AimYaw);


	void PerformInteractionCheck();
	
	void FoundInteractable(AActor* NewInteractable);

	void NoInteractableFound();
	
	void BeginInteract();

	void EndInteract();

	void Interact();

	void FindTargetInteractableInfo();

	void WeaponBuyInteract();

	UFUNCTION(Server, Unreliable)
		void SpawnImpactEffect_Server(UParticleSystem* _ImpactEffect, FVector _Location, FRotator _Rotation);

	UFUNCTION(NetMulticast, Unreliable)
		void SpawnImpactEffect_NetMulticast(UParticleSystem* _ImpactEffect, FVector _Location, FRotator _Rotation);

	UFUNCTION(Server, Unreliable)
		void SpawnBloodEffect_Server(FVector _Location, FRotator _Rotation);

	UFUNCTION(NetMulticast, Unreliable)
		void SpawnBloodEffect_NetMulticast(FVector _Location, FRotator _Rotation);



private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UInputConfig> PlayerCharacterInputConfigData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TSubclassOf<class UCameraShakeBase> FireShake;

	UPROPERTY(meta = (AllowPrivateAccess = true))
		TObjectPtr<class UPlayerAnimInstance> AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateaccess = true))
		UDataTable* PurchasableItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MaxSprayAngle;


	// Zoom 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
		float GunRange = 5000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
		float TargetFOV = 70.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
		float CurrentFOV = 70.f;
	//
	 
	// Single / Burst
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess))
		float FirePerMinute = 600;

	FTimerHandle BetweenShotsTimer;

	float TimeBetWeenFire;

	bool bIsBurstTrigger = false;
	//

	//Death
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess))
		uint8 bIsDead : 1;
	//

	//Reload
	UFUNCTION(Server, Unreliable)
		void PlayReloadMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
		void PlayReloadMontage_NetMulticast();

	void ReloadAnimationPlay();
	

	//interaction
	
	UPROPERTY(VisibleAnywhere)
		TScriptInterface<IInteractionInterface> TargetInteractable;

	float InteractionCheckFrequency = 0.1f;

	float InteractionCheckRange = 400.0f;

	FTimerHandle InteractionTimerHandle;

	FInteractionData InteractionData;

	FWeaponData FindTargetWeaponData;

	//

	AWeapon* interactableWeapon;

	FTimeline RecoilTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		class UCurveFloat* VerticalCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		class UCurveFloat* HorizontalCurve;

	float PitchInput;
	float YawInput;

	FRotator StartRotation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		float RandomSpreadValue = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
		UParticleSystem* BloodEffect;

	//Reload

	UFUNCTION(Server, Unreliable)
		void PlayWeaponChangeMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
		void PlayWeaponChangeMontage_NetMulticast();

	UFUNCTION(Server, Reliable)
		void ChangeWeaponActive_Server();

	void WeaponChangeAnimationPlay();

	void ChangeWeapon();

	void ChangeWeaponActive();



	//

	UFUNCTION(Server, Unreliable)
		void PlayWeaponSwapMontage_Server();

	UFUNCTION(NetMulticast, Unreliable)
		void PlayWeaponSwapMontage_NetMulticast();

	UFUNCTION(Server, Reliable)
		void SetSwapWeaponActive_Server();

	void WeaponSwapAnimationPlay();

	void Swap_1();

	void Swap_2();

	void SwapWeapon();

	void SetSwapWeaponActive();

	//

	//

	UFUNCTION(Server, Reliable)
		void PlayThrowGrenadeMontage_Server();

	UFUNCTION(NetMulticast, Reliable)
		void PlayThrowGrenadeMontage_NetMulticast();

	float Grenade_Distance = 0.f;

	void ThrowGrenadeAnimationPlay();

	void ThrowGrenade_Tick();

	void ThrowGrenadeEnd();

	UFUNCTION(Server, Reliable)
		void ThrowGrenadeEnd_Server();

	UFUNCTION(Server, Reliable)
		void ThrowGrenadeSpawn_Server(FVector _LaunchVecter);

	UFUNCTION(NetMulticast, Reliable)
		void ThrowGrenadeEnd_NetMulticast();


	void ThrowGrenadeAnimationEnd();


	FVector LaunchVecter;
};	

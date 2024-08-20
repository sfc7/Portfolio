// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MonsterCharacter.h"
#include "ZombieCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AZombieCharacter : public AMonsterCharacter
{
	GENERATED_BODY()

	friend class UBTTask_Attack;
public:
	AZombieCharacter();

	void ZombieHitted(class APlayerCharacter* Player, FHitResult _HitResult);

	class UMonsterComponent* GetMonsterComponent() { return MonsterComponent; }

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void MeshAssetLoad();

	void Attack();

	void Attack_BasicHit();

	void AttackMontageEnd();

	void DestroyActor();

	uint16 GetMoneyFromHitPart(FString BoneName);

	UFUNCTION(NetMulticast, Reliable)
		void IsDead_NetMulticast();

	UFUNCTION()
		void OnHittedRagdollRestoreTimerElapsed();

	UFUNCTION(NetMulticast, Unreliable)
		void PlayRagdoll_NetMulticast();

	uint8 GetHitPart(FString BoneName);

	uint8 GetMoneyFromHitPart(uint8 HitPart);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UMonsterComponent> MonsterComponent;
private:
	FSoftObjectPath	CurrentZombieCharacterMeshPath = FSoftObjectPath();

	TSharedPtr<struct FStreamableHandle> AssetStreamableHandle = nullptr;

	UPROPERTY()
		TObjectPtr<class UZombieAnimInstance> ZombieAnimInstance;

	//Attack
	float AttackDamage = 10.f;
	
	float AttackRange = 50.f;

	float AttackRadius = 50.f;
	//

	FTimerHandle HittedRagdollRestoreTimer;

	FTimerDelegate HittedRagdollRestoreTimerDelegate;

	float TargetRagDollPercent = 0.f;

	float CurrentRagDollPercent = 0.f;

	bool bIsRagdoll = false;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/MonsterAnimInstance.h"
#include "ZombieAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FAttackHit);
DECLARE_MULTICAST_DELEGATE(FAttackMontageEnd);
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UZombieAnimInstance : public UMonsterAnimInstance
{
	GENERATED_BODY()
public:
	UZombieAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
		void AnimNotify_AttackHit();

	UFUNCTION()
		void AnimNotify_AttackMontageEnd();

	void PlayAttackMontage();



public:
	FAttackHit AttackHit;
	FAttackMontageEnd AttackMontageEnd;



private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TObjectPtr<class AZombieCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float GroundSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage;


};

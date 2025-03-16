// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/MonsterAnimInstance.h"
#include "ZombieAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FAttackHit);
DECLARE_MULTICAST_DELEGATE(FAttackMontageEnd);
DECLARE_MULTICAST_DELEGATE(FDeathMontageEnd);
DECLARE_MULTICAST_DELEGATE(FNavLinkJumpinghMoveLocation);
DECLARE_MULTICAST_DELEGATE(FNavLinkJumpinghMontageEnd);
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UZombieAnimInstance : public UMonsterAnimInstance
{
	GENERATED_BODY()
public:
	UZombieAnimInstance();

	UFUNCTION()
		void AnimNotify_AttackHit();

	UFUNCTION()
		void AnimNotify_AttackMontageEnd();

	UFUNCTION()
		void AnimNotify_DeathMontageEnd();

	UFUNCTION()
		void AnimNotify_NavLinkJumpinghMoveLocation();

	UFUNCTION()
		void AnimNotify_NavLinkJumpinghMontageEnd();

	void PlayDeathMontage();

	void PlayNavLinkJumpinghMontage();

	void PlayNavLinkFlailMontage();

	void PlayAttackMontage(int16 _MontageArrayNum);
		

protected:

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


public:
	FAttackHit AttackHit;
	FAttackMontageEnd AttackMontageEnd;
	FDeathMontageEnd DeathMontageEnd;
	FNavLinkJumpinghMoveLocation NavLinkJumpinghMoveLocation;
	FNavLinkJumpinghMontageEnd NavLinkJumpinghMontageEnd;

	int16 AttackMontageArrayNum;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TObjectPtr<class AZombieCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		int32 RandomWalkBlendSpace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = true))
		UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = true))
		UAnimMontage* NavLinkJumpingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = true))
		UAnimMontage* NavLinkFlailMontage;

	TArray<UAnimMontage*> AttackMontageArray;

};

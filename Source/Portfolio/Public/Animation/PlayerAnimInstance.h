// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Component/CharacterComponent.h"
#include "PlayerAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FFireMontage);
DECLARE_MULTICAST_DELEGATE(FFireMontageEnd);
/**
 * 
 */
UCLASS()
class PORTFOLIO_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPlayerAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void PlayRifleFireAnimMontage();

	void PlayRifleIronSightFireAnimMontage();

	UFUNCTION()
		void AnimNotify_FireMontage();

	UFUNCTION()
		void AnimNotify_FireMontageEnd();

public:
	FFireMontage FireMontage;
	FFireMontageEnd FireMontageEnd;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TObjectPtr<class APlayerCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		FVector MoveInputWithMaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		FVector MoveInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = USTPSAnimInstance, meta = (AllowPrivateAccess = true))
		FRotator ControlRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = USTPSAnimInstance, meta = (AllowPrivateAccess = true))
		uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = USTPSAnimInstance, meta = (AllowPrivateAccess = true))
		uint8 bIsDead : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = USTPSAnimInstance, meta = (AllowPrivateAccess = true))
		uint8 bIsAiming : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = USTPSAnimInstance, meta = (AllowPrivateAccess = true))
		ECurrentState CurrentState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = true))
		TObjectPtr<class UAnimMontage> RifleFireAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = true))
		TObjectPtr<class UAnimMontage> RifleIronSightFireAnimMontage;
	
};	
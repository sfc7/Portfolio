// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ZombieAnimInstance.h"
#include "Character/ZombieCharacter.h"
#include "Component/CharacterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UZombieAnimInstance::UZombieAnimInstance()
{
	
}

void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<AZombieCharacter>(GetOwningActor());

	if (IsValid(OwnerCharacter)) {
		MovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwnerCharacter) && IsValid(MovementComponent)) {
		GroundSpeed = MovementComponent->GetLastUpdateVelocity().Size();
	}
}

void UZombieAnimInstance::AnimNotify_AttackHit()
{
	AttackHit.Broadcast();
}

void UZombieAnimInstance::AnimNotify_AttackMontageEnd()
{
	AttackMontageEnd.Broadcast();
}

void UZombieAnimInstance::AnimNotify_DeathMontageEnd()
{
	DeathMontageEnd.Broadcast();
}

void UZombieAnimInstance::PlayAttackMontage()
{
	if (IsValid(AttackMontage)) {
		if (!Montage_IsPlaying(AttackMontage)) {
			Montage_Play(AttackMontage);
		}
	}
}

void UZombieAnimInstance::PlayDeathMontage()
{
	if (IsValid(DeathMontage)) {
		StopAllMontages(0.0f);
		if (!Montage_IsPlaying(DeathMontage)) {
			Montage_Play(DeathMontage);
		}
	}
}

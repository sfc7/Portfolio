// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ZombieAnimInstance.h"
#include "Character/ZombieCharacter.h"
#include "Component/CharacterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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

	AttackMontageArray = { AttackMontage1, AttackMontage2, AttackMontage3 };

	AttackMontageArrayNum = AttackMontageArray.Num();
}

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(OwnerCharacter) && IsValid(MovementComponent)) {
		GroundSpeed = MovementComponent->GetLastUpdateVelocity().Size();
		RandomWalkBlendSpace = OwnerCharacter->GetRandomWalkBlendSpace();
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

void UZombieAnimInstance::AnimNotify_NavLinkJumpinghMoveLocation()
{
	NavLinkJumpinghMoveLocation.Broadcast();
}

void UZombieAnimInstance::AnimNotify_NavLinkJumpinghMontageEnd()
{
	NavLinkJumpinghMontageEnd.Broadcast();
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

void UZombieAnimInstance::PlayNavLinkJumpinghMontage()
{
	if (IsValid(NavLinkJumpingMontage)) {
		if (!Montage_IsPlaying(NavLinkJumpingMontage)) {
			Montage_Play(NavLinkJumpingMontage);
		}
	}
}

void UZombieAnimInstance::PlayNavLinkFlailMontage()
{
	if (IsValid(NavLinkFlailMontage)) {
		if (!Montage_IsPlaying(NavLinkFlailMontage)) {
			Montage_Play(NavLinkFlailMontage);
		}
	}
}

void UZombieAnimInstance::PlayAttackMontage(int16 _MontageArrayNum)
{
	if (IsValid(AttackMontageArray[_MontageArrayNum]))
	{
		if (!Montage_IsPlaying(AttackMontageArray[_MontageArrayNum])) {
			Montage_Play(AttackMontageArray[_MontageArrayNum]);
		}
	}

}
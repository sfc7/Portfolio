// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/PlayerAnimInstance.h"
#include "Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Component/CharacterComponent.h"

UPlayerAnimInstance::UPlayerAnimInstance() : bIsDead(false)
{
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<APlayerCharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter))
	{
		MovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(MovementComponent) && IsValid(OwnerCharacter))
	{
		GroundSpeed = MovementComponent->GetLastUpdateVelocity().Size();

		float ForwardInputValue = fabs(MovementComponent->Velocity.X) * OwnerCharacter->GetForwardInputValue();
		float RightInputValue = fabs(MovementComponent->Velocity.Y) * OwnerCharacter->GetRightInputValue();
		float UpInputValue = MovementComponent->Velocity.Z;
		MoveInputWithMaxSpeed = FVector(ForwardInputValue, RightInputValue, UpInputValue);

		float X = fabs(MoveInputWithMaxSpeed.X) < KINDA_SMALL_NUMBER ? 0.f : MoveInputWithMaxSpeed.X / fabs(MoveInputWithMaxSpeed.X);
		float Y = fabs(MoveInputWithMaxSpeed.Y) < KINDA_SMALL_NUMBER ? 0.f : MoveInputWithMaxSpeed.Y / fabs(MoveInputWithMaxSpeed.Y);
		float Z = fabs(MoveInputWithMaxSpeed.Z) < KINDA_SMALL_NUMBER ? 0.f : MoveInputWithMaxSpeed.Z / fabs(MoveInputWithMaxSpeed.Z);
		MoveInput = FVector{ X,Y,Z };

		ControlRotation.Pitch = OwnerCharacter->GetAimPitch();
		ControlRotation.Yaw = OwnerCharacter->GetAimYaw();


		bIsFalling = MovementComponent->IsFalling();
		bIsAiming = OwnerCharacter->IsAiming();
		bIsDead = OwnerCharacter->IsDead();
		CurrentState = OwnerCharacter->IsCurrentState();
	}
}
void UPlayerAnimInstance::PlayRifleFireAnimMontage()
{
	if (!Montage_IsPlaying(RifleFireAnimMontage)) {
		Montage_Play(RifleFireAnimMontage);
	}
}

void UPlayerAnimInstance::PlayRifleIronSightFireAnimMontage()
{
	if (!Montage_IsPlaying(RifleIronSightFireAnimMontage)) {
		Montage_Play(RifleIronSightFireAnimMontage);
	}
}

void UPlayerAnimInstance::PlayRifleReloadAnimMontage()
{
	if (!Montage_IsPlaying(RifleReloadAnimMontage)) {
		Montage_Play(RifleReloadAnimMontage);
	}
}

void UPlayerAnimInstance::PlayRifleIronSightReloadAnimMontage()
{
	if (!Montage_IsPlaying(RifleIronReloadAnimMontage)) {
		Montage_Play(RifleIronReloadAnimMontage);
	}
}

void UPlayerAnimInstance::PlayChangeWeaponAnimMontage()
{
	if (!Montage_IsPlaying(ChangeWeaponAnimMontage)) {
		Montage_Play(ChangeWeaponAnimMontage);
	}
}

void UPlayerAnimInstance::PlayHitAnimMontage()
{
	if (IsValid(HitAnimMontage)) {
		StopAllMontages(0.0f);
		if (!Montage_IsPlaying(HitAnimMontage)) {
			Montage_Play(HitAnimMontage);
		}
	}
}

void UPlayerAnimInstance::AnimNotify_FireMontage()
{
	FireMontage.Broadcast();	
}

void UPlayerAnimInstance::AnimNotify_FireMontageEnd()
{
	FireMontageEnd.Broadcast();
}

void UPlayerAnimInstance::AnimNotify_ReloadMontage()
{
	ReloadMontage.Broadcast();
}

void UPlayerAnimInstance::AnimNotify_ReloadMontageEnd()
{
	ReloadMontageEnd.Broadcast();
}

void UPlayerAnimInstance::AnimNotify_ChangeWeaponMontage()
{
	ChangeWeaponMontage.Broadcast();
}


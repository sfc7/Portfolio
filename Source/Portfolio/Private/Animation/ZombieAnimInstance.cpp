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

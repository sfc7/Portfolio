// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/LobbyCharacter.h"
#include "Camera/CameraComponent.h"

ALobbyCharacter::ALobbyCharacter()
{
	CameraComponent->SetupAttachment(RootComponent);
	/*CameraComponent->SetRelativeLocation(FVector(180.f, 0.f, 0.f));*/
	CameraComponent->SetAbsolute(true, true, false);
	CameraComponent->SetWorldLocation(FVector(600.f, 250.f, 90.f));
	CameraComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	CameraComponent->FieldOfView = 45.f;

}

void ALobbyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();

}
			
// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraComponent.h"
#include "Character/LobbyCharacter.h"


ALobbyCharacter::ALobbyCharacter()
{
	CameraComponent->SetupAttachment(RootComponent);
	/*CameraComponent->SetRelativeLocation(FVector(180.f, 0.f, 0.f));*/
	CameraComponent->SetAbsolute(true, true, false);
	CameraComponent->SetWorldLocation(FVector(1700.f, 1000.f, 45.f));
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

	FTimerHandle StateTimer;
	GetWorld()->GetTimerManager().SetTimer(StateTimer, FTimerDelegate::CreateLambda([&] {
		GetMesh()->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	}), 0.2f, false);
}
				
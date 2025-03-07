// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MonsterCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMonsterCharacter::AMonsterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FVector PivotPosition(0.f, 0.f, -88.0f);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);
}

// Called when the game starts or when spawned
void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMonsterCharacter::JumpInNavLink(FVector _Destination, float Jumpheight)
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = _Destination + FVector(0.f, 0.f, Jumpheight);
	FVector OutResultVector;

	bool bProjectileVelocity = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,	
		OUT OutResultVector,
		StartLocation,
		EndLocation
	);

	LaunchCharacter(OutResultVector,true , true);
}

// Called every frame
void AMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMonsterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/ZombieMantleNavLink.h"

AZombieMantleNavLink::AZombieMantleNavLink()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AZombieMantleNavLink::BeginPlay()
{
	Super::BeginPlay();
}

void AZombieMantleNavLink::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/ZombieSpawnPoint.h"

AZombieSpawnPoint::AZombieSpawnPoint()
{
	bIsUsed = false;
}

bool AZombieSpawnPoint::IsUsed()
{
	return bIsUsed;
}

void AZombieSpawnPoint::SetUsed(bool _bIsUsed)
{
	bIsUsed = _bIsUsed;
}

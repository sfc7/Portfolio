// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerStateSave.h"

UPlayerStateSave::UPlayerStateSave()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> DefaultMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));

	CurrentLevel = 1;
	CurrentEXP = 0;
	Money = 0;


	PlayerMesh = nullptr;
	if (DefaultMeshRef.Succeeded()) {
		PlayerMesh = DefaultMeshRef.Object;
	}
}
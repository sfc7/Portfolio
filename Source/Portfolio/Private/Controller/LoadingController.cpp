// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/LoadingController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

void ALoadingController::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM)) {
		FString NextLevel = UGameplayStatics::ParseOption(GM->OptionsString, FString(TEXT("NextLevel")));

		UGameplayStatics::OpenLevel(GM, *NextLevel, false);
	}
}


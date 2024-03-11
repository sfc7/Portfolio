// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menu_UI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Controller/PlayerCharacterController.h"

void UMenu_UI::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnResumeButtonClicked);
	TitleButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnTitleButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}


void UMenu_UI::OnResumeButtonClicked()
{
	APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetOwningPlayer());
	if (IsValid(PlayerController)) {
		PlayerController->ToggleMenu();
	}
}
	
void UMenu_UI::OnTitleButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Lobby")));
}

void UMenu_UI::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}


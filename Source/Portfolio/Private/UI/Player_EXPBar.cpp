// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player_EXPBar.h"
#include "Components/ProgressBar.h"
#include "Game/FPlayerState.h"
#include "Character/PlayerCharacter.h"

void UPlayer_EXPBar::SetMaxExp(float _MaxEXP)
{
	SetMaxFigure(_MaxEXP);
}

void UPlayer_EXPBar::InitalizeEXPBarWidget(AFPlayerState* NewPlayerState)
{
	OnCurrentEXPChange(NewPlayerState->GetCurrentEXP());
}

void UPlayer_EXPBar::OnCurrentEXPChange(float NewEXP)
{
	if (IsValid(Bar)) {
		if (MaxFigure > KINDA_SMALL_NUMBER) {
			Bar->SetPercent(NewEXP / MaxFigure);
			//UE_LOG(LogTemp, Log, TEXT("if NewEXP : %f"), NewEXP);
			//UE_LOG(LogTemp, Log, TEXT("if percent : %f"), NewEXP / MaxFigure);
			//UE_LOG(LogTemp, Log, TEXT("if MaxFigure : %f"), MaxFigure);
		}
		else {
			Bar->SetPercent(0.f);
			//UE_LOG(LogTemp, Log, TEXT("else NewEXP : %f"), NewEXP);
			//UE_LOG(LogTemp, Log, TEXT("else percent : %f"), NewEXP / MaxFigure);
			//UE_LOG(LogTemp, Log, TEXT("else MaxFigure : %f"), MaxFigure);
		}
	}
}

void UPlayer_EXPBar::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerCharacter* OwningCharacter = Cast<APlayerCharacter>(OwningActor);
	if (IsValid(OwningCharacter)) {
		OwningCharacter->SetWidget(this);
	}
}

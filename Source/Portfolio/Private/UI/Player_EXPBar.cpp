// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player_EXPBar.h"
#include "Components/ProgressBar.h"
#include "Game/FPlayerState.h"
#include "Character/PlayerCharacter.h"
#include "Component/CharacterComponent.h"

void UPlayer_EXPBar::SetMaxExp(float _MaxEXP)
{
	SetMaxFigure(_MaxEXP);
}

void UPlayer_EXPBar::InitalizeEXPBarWidget(UCharacterComponent* _CharacterComponent)
{
	OnCurrentEXPChange(_CharacterComponent->GetCurrentEXP());
}

void UPlayer_EXPBar::OnCurrentEXPChange(float NewEXP)
{
	FString Message = FString::Printf(TEXT("OnCurrentEXPChange : %f"), NewEXP);

	if (IsValid(Bar)) {
		if (MaxFigure > KINDA_SMALL_NUMBER) {
			Bar->SetPercent(NewEXP / MaxFigure);
		}
		else {
			Bar->SetPercent(0.f);
		}
	}
}

void UPlayer_EXPBar::OnMaxEXPChange(float NewMaxEXP)
{
	MaxFigure = NewMaxEXP;
}

void UPlayer_EXPBar::NativeConstruct()
{
	Super::NativeConstruct();
}

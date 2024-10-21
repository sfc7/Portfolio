// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player_HPBar.h"
#include "Components/ProgressBar.h"
#include "Component/CharacterComponent.h"
#include "Character/PlayerCharacter.h"

void UPlayer_HPBar::SetMaxHp(int _MaxHp)
{
	SetMaxFigure(_MaxHp);
}

void UPlayer_HPBar::InitalizeHPBarWidget(UCharacterComponent* NewComponent)
{
	OnCurrentHpChange(0, NewComponent->GetCurrentHp());
}

void UPlayer_HPBar::OnMaxHpChange(int _MaxHp, int NewMaxHp)
{
	SetMaxFigure(NewMaxHp);

	OnCurrentHpChange(0, NewMaxHp);
}

void UPlayer_HPBar::OnCurrentHpChange(int _Hp, int NewHp)
{
	if (IsValid(Bar)) {
		if (MaxFigure > KINDA_SMALL_NUMBER) {
			Bar->SetPercent((float)NewHp / (float)MaxFigure);
		}
		else {
			Bar->SetPercent(0.f);
		}
	}
	
}

void UPlayer_HPBar::NativeConstruct()
{
	Super::NativeConstruct();

}

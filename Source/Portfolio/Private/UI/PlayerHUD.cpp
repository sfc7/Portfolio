// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUD.h"
#include "Component/CharacterComponent.h"
#include "UI/Player_HPBar.h"
#include "UI/Player_EXPBar.h"
#include "Components/TextBlock.h"
#include "Game/FGameInstance.h"
#include "Game/FPlayerState.h"

void UPlayerHUD::BindCharacterComponent(UCharacterComponent* _CharacterComponent)
{
	if (IsValid(_CharacterComponent))
	{
		CharacterComponent = _CharacterComponent;
		CharacterComponent->OnCurrnetHpChangeDelegate.AddDynamic(Hp_Bar, &UPlayer_HPBar::OnCurrentHpChange);
		CharacterComponent->OnMaxHpChangeDelegate.AddDynamic(Hp_Bar, &UPlayer_HPBar::OnMaxHpChange);

		UFGameInstance* GameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
		if (IsValid(GameInstance)) {
			if (nullptr != GameInstance->GetCharacterTable() || nullptr != GameInstance->GetCharacterTableRowFromLevel(1)) {
				float MaxHp = GameInstance->GetCharacterTableRowFromLevel(1)->MaxHp;
				Hp_Bar->SetMaxHp(MaxHp);
				Hp_Bar->InitalizeHPBarWidget(CharacterComponent.Get());
			}
		}
	}
}

void UPlayerHUD::BindPlayerState(AFPlayerState* _PlayerState)
{
	if (IsValid(_PlayerState)) {
		PlayerState = _PlayerState;
		PlayerState->OnCurrentEXPChangedDelegate.AddDynamic(Exp_Bar, &UPlayer_EXPBar::OnCurrentEXPChange);
		PlayerState->OnMaxEXPChangedDelegate.AddDynamic(Exp_Bar, &UPlayer_EXPBar::OnMaxEXPChange);
		PlayerState->OnCurrentLevelChangedDelegate.AddDynamic(this, &ThisClass::LevelTextChange);
	}

	UFGameInstance* GameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GameInstance)) {
		if (nullptr != GameInstance->GetCharacterTable() || nullptr != GameInstance->GetCharacterTableRowFromLevel(1)) {
			float TableMaxExp = GameInstance->GetCharacterTableRowFromLevel(1)->MaxEXP;
			Exp_Bar->SetMaxExp(TableMaxExp);
			Exp_Bar->InitalizeEXPBarWidget(PlayerState.Get());
			LevelTextChange(PlayerState->GetCurrentLevel());
		}
	}


}

void UPlayerHUD::LevelTextChange(int32 NewLevel)
{
	FString ConvertString = FString::Printf(TEXT("Lv. %d"), NewLevel);

	LevelText->SetText(FText::FromString(ConvertString));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUD.h"
#include "Component/CharacterComponent.h"
#include "UI/Player_HPBar.h"
#include "Components/TextBlock.h"
#include "Game/FGameInstance.h"


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

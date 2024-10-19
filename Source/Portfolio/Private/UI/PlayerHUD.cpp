// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PlayerHUD.h"
#include "Component/CharacterComponent.h"
#include "UI/Player_HPBar.h"
#include "UI/Player_EXPBar.h"
#include "Components/TextBlock.h"
#include "Game/FGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Game/FPlayerState.h"


void UPlayerHUD::BindCharacterComponent(UCharacterComponent* _CharacterComponent)
{
	if (IsValid(_CharacterComponent))
	{
		CharacterComponent = _CharacterComponent;
		CharacterComponent->OnCurrnetHpChangeDelegate.AddDynamic(Hp_Bar, &UPlayer_HPBar::OnCurrentHpChange);
		CharacterComponent->OnCurrnetHpChangeDelegate.AddDynamic(this, &ThisClass::CurrntHpTextChange);
		CharacterComponent->OnMaxHpChangeDelegate.AddDynamic(Hp_Bar, &UPlayer_HPBar::OnMaxHpChange);
		CharacterComponent->OnCurrentAmmoAndTotalAmmoChangeDelegate.AddDynamic(this, &ThisClass::CurrentAmmoAndTotalAmmoChange);
		CharacterComponent->OnCurrentEXPChangedDelegate.AddDynamic(Exp_Bar, &UPlayer_EXPBar::OnCurrentEXPChange);
		CharacterComponent->OnMaxEXPChangedDelegate.AddDynamic(Exp_Bar, &UPlayer_EXPBar::OnMaxEXPChange);
		CharacterComponent->OnCurrentLevelChangedDelegate.AddDynamic(this, &ThisClass::LevelTextChange);
		CharacterComponent->OnMoneyChangeDelegate.AddDynamic(this, &ThisClass::MoneyChange);

		UFGameInstance* GameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
		if (IsValid(GameInstance)) {
			if (nullptr != GameInstance->GetCharacterTable() || nullptr != GameInstance->GetCharacterTableRowFromLevel(1)) {
				int MaxHp = GameInstance->GetCharacterTableRowFromLevel(1)->MaxHp;
				Hp_Bar->SetMaxHp(MaxHp);
				Hp_Bar->InitalizeHPBarWidget(CharacterComponent.Get());
			}
		}
	}

	UFGameInstance* GameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GameInstance)) {
		if (nullptr != GameInstance->GetCharacterTable() || nullptr != GameInstance->GetCharacterTableRowFromLevel(1)) {
			float TableMaxExp = GameInstance->GetCharacterTableRowFromLevel(1)->MaxEXP;
			Exp_Bar->SetMaxExp(TableMaxExp);
			Exp_Bar->InitalizeEXPBarWidget(CharacterComponent.Get());
			LevelTextChange(CharacterComponent->GetCurrentLevel()); 

		}
	}

	BindRenewal(_CharacterComponent);
}

void UPlayerHUD::BindPlayerState(AFPlayerState* _PlayerState)
{
}

void UPlayerHUD::LevelTextChange(int32 NewLevel)
{
	FString ConvertString = FString::Printf(TEXT("Lv. %d"), NewLevel);

	LevelText->SetText(FText::FromString(ConvertString));
}

void UPlayerHUD::MoneyChange(int32 _Money)
{
	FString ConvertString = FString::Printf(TEXT("%d"), _Money);

	Money->SetText(FText::FromString(ConvertString));
}

void UPlayerHUD::CurrntHpTextChange(int32 _Hp, int32 NewHp)
{
	FString ConvertString = FString::Printf(TEXT("%d"), NewHp);

	CurrntHpText->SetText(FText::FromString(ConvertString));

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("CurrntHpTextChange : %s"), *ConvertString)); 
}

void UPlayerHUD::CurrentAmmoAndTotalAmmoChange(int32 _CurrentAmmo, int32 _TotalAmmo)
{
	FString ConvertString = FString::Printf(TEXT("%d / %d"), _CurrentAmmo, _TotalAmmo);

	CurrentAmmo->SetText(FText::FromString(ConvertString));
}

void UPlayerHUD::BindRenewal(UCharacterComponent* _CharacterComponent)
{
	Hp_Bar->OnCurrentHpChange(0, _CharacterComponent->GetCurrentHp());
	Hp_Bar->OnMaxHpChange(0, CharacterComponent->GetMaxHp());
	CurrentAmmoAndTotalAmmoChange(CharacterComponent->GetCurrentAmmo(), CharacterComponent->GetTotalAmmo());
	Exp_Bar->OnCurrentEXPChange(CharacterComponent->GetCurrentEXP());
	Exp_Bar->OnMaxEXPChange(CharacterComponent->GetMaxEXP());
	LevelTextChange(CharacterComponent->GetCurrentLevel());
	MoneyChange(CharacterComponent->GetMoney());
	CurrntHpTextChange(CharacterComponent->GetCurrentHp(), CharacterComponent->GetCurrentHp());

	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BindRenewal : %d"), CharacterComponent->GetCurrentHp()));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FPlayerState.h"
#include "Game/FGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Game/PlayerStateSave.h"


AFPlayerState::AFPlayerState()
{
}

void AFPlayerState::InitPlayerState()
{
	FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {
		if (nullptr != FGameInstance->GetCharacterTable() || nullptr != FGameInstance->GetCharacterTableRowFromLevel(1)) {
			MaxLevel = FGameInstance->GetCharacterTable()->GetRowMap().Num();
			CurrentLevel = 1;
			CurrentEXP = 0;
			MaxEXP = FGameInstance->GetCharacterTableRowFromLevel(1)->MaxEXP;
		}
	}
}


void AFPlayerState::SetCurrentLevel(int32 _CurrentLevel)
{
	int32 ClampLevel = FMath::Clamp(_CurrentLevel, 1, MaxLevel);

	FCharacterTable* RowData = FGameInstance->GetCharacterTableRowFromLevel(ClampLevel);
	if (nullptr != RowData) {
		MaxEXP = RowData->MaxEXP;
		CurrentLevel = ClampLevel;
		OnCurrentLevelChangedDelegate.Broadcast(CurrentLevel);
		OnMaxEXPChangedDelegate.Broadcast(MaxEXP);
	}
}

void AFPlayerState::SetCurrentEXP(float _CurrentEXP)
{
	CurrentEXP = _CurrentEXP;

	while (MaxEXP - KINDA_SMALL_NUMBER < CurrentEXP) { 
		CurrentEXP -= MaxEXP;
		SetCurrentLevel(GetCurrentLevel() + 1);
			
		if (CurrentEXP < 0) {
			CurrentEXP = 0.f;
		}
	}
	
	OnCurrentEXPChangedDelegate.Broadcast(CurrentEXP);
}



void AFPlayerState::SetPlayerNumber(int32 _PlayerNumber)
{
	PlayerNumber = _PlayerNumber;
}

void AFPlayerState::SetMoney(int32 _Money)
{
	PlayerMoney = _Money;
	
	OnMoneyChangeDelegate.Broadcast(_Money);
	UE_LOG(LogTemp, Log, TEXT("%d"), PlayerMoney);
}

void AFPlayerState::SetReloadMaxAmmo(int32 _ReloadMaxAmmo)
{
	ReloadMaxAmmo = _ReloadMaxAmmo;
}

void AFPlayerState::SetTotalAmmo(int32 _TotalAmmo)
{
	TotalAmmo = _TotalAmmo;
}

void AFPlayerState::SetCurrentAmmo(int32 _CurrentAmmo)
{
	if (_CurrentAmmo >= 0) {
		CurrentAmmo = _CurrentAmmo;
	}
	OnCurrentAmmoChangeDelegate.Broadcast(CurrentAmmo);
}

void AFPlayerState::SetCurrentAndTotalAmmo(int32 _CurrentAmmo, int32 _TotalAmmo)
{
	CurrentAmmo = FMath::Max(0.f, _CurrentAmmo);
	TotalAmmo = FMath::Max(0.f, _TotalAmmo);

	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, _TotalAmmo);
}


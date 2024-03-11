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
	int32 PreCurrentLevel = CurrentLevel;
	CurrentLevel = FMath::Clamp(_CurrentLevel, 1, MaxLevel);

	FCharacterTable* RowData = FGameInstance->GetCharacterTableRowFromLevel(CurrentLevel);
	if (nullptr != RowData) {
		MaxEXP = RowData->MaxEXP;
		OnCurrentLevelChangedDelegate.Broadcast(PreCurrentLevel, CurrentLevel);
	}
}

void AFPlayerState::SetCurrentEXP(float _CurrentEXP)
{
	float PreCurrentExp = CurrentEXP;
	CurrentEXP = FMath::Clamp<float>(_CurrentEXP, 0.f, MaxEXP);

	if (MaxEXP - KINDA_SMALL_NUMBER < CurrentEXP) {
		SetCurrentLevel(CurrentLevel + 1);
		CurrentEXP = 0.f;
	}

	OnCurrentEXPChangedDelegate.Broadcast(PreCurrentExp, CurrentEXP);
}

void AFPlayerState::SetPlayerNumber(int32 _PlayerNumber)
{
	PlayerNumber = _PlayerNumber;
}

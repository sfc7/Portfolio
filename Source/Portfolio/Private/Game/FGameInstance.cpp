// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FGameInstance.h"

void UFGameInstance::Init()
{
	Super::Init();

	//if (IsValid(CharacterTable) && CharacterTable->GetRowMap().Num() > 0) {
	//	for (int32 i = 1; i <= CharacterTable->GetRowMap().Num(); i++) {
	//		check(nullptr != GetCharacterTableRowFromLevel(i));
	//	}
	//}
}

void UFGameInstance::Shutdown()
{
	Super::Shutdown();
}

FCharacterTable* UFGameInstance::GetCharacterTableRowFromLevel(int32 Level)
{
	if (IsValid(CharacterTable)) {
		return CharacterTable->FindRow<FCharacterTable>(*FString::FromInt(Level), TEXT(""));
	}

	return nullptr;
}

FMonsterTable* UFGameInstance::GetMonsterTableRowFromName(FName _RowName)
{
	if (IsValid(MonsterTable)) {			
		return MonsterTable->FindRow<FMonsterTable>(_RowName, TEXT(""));
	}

	return nullptr;
}

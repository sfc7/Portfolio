// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FPlayerState.h"
#include "Game/FGameInstance.h"
#include "Game/FGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Game/PlayerStateSave.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AFPlayerState::AFPlayerState()
{
	
}

void AFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReloadMaxAmmo);
	DOREPLIFETIME(ThisClass, TotalAmmo);
	DOREPLIFETIME(ThisClass, CurrentAmmo);
}

void AFPlayerState::InitPlayerState()
{
	/*UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("InitPlayerState")), true, true, FLinearColor::Red, 10.0f);*/
	FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {
		if (nullptr != FGameInstance->GetCharacterTable() || nullptr != FGameInstance->GetCharacterTableRowFromLevel(1)) {
			MaxLevel = FGameInstance->GetCharacterTable()->GetRowMap().Num();
			CurrentLevel = 1;
			CurrentEXP = 0;
			PlayerMoney = 0;
			ReloadMaxAmmo = 0; 
			TotalAmmo = FGameInstance->TotalAmmo;
			CurrentAmmo = FGameInstance->CurrentAmmo;
			MaxEXP = FGameInstance->GetCharacterTableRowFromLevel(1)->MaxEXP;
		}/*
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("FGameInstance Ammo : % d"), FGameInstance->TotalAmmo), true, true, FLinearColor::Red, 10.0f);
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("PlayerState Ammo : % d"), TotalAmmo), true, true, FLinearColor::Blue, 10.0f);*/
		UE_LOG(LogTemp, Log, TEXT("InitPlayerState Ammo : %d"), TotalAmmo);
		UE_LOG(LogTemp, Log, TEXT("InitPlayerState CurrentAmmo : %d"), CurrentAmmo);
	}
}

void AFPlayerState::SaveFGameInstance()
{
	AController* Controller = Cast<AController>(GetOwner());
	if (Controller && Controller->IsLocalController())
	{ 
		//if (UGameplayStatics::SaveGameToSlot(PlayerStateSave, SaveSlotName, 0)) {
		//	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Saved.")));
		//}

		//UE_LOG(LogTemp, Log, TEXT("------Save----"));
		//UE_LOG(LogTemp, Log, TEXT("Total Ammo : %d, Current Ammo : %d"), TotalAmmo, CurrentAmmo);
		//UE_LOG(LogTemp, Log, TEXT("Flag  % d"), PlayerStateSaveFlag);
		//UE_LOG(LogTemp, Log, TEXT("--------------"));
	}

}
	
void AFPlayerState::SaveInitValueFromPlayerStateSave()
{
	//UE_LOG(LogTemp, Log, TEXT("Load : %s"), *SaveSlotName);

	//UPlayerStateSave* PlayerStateSave = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	//if (!IsValid(PlayerStateSave)) {
	//	PlayerStateSave = GetMutableDefault<UPlayerStateSave>();
	//}
	//else {
	//	PlayerStateSaveFlag = true;

	//	SetCurrentLevel(PlayerStateSave->CurrentLevel);
	//	SetCurrentEXP(PlayerStateSave->CurrentEXP);
	//	SetCurrentAndTotalAmmo(PlayerStateSave->CurrentAmmo, PlayerStateSave->TotalAmmo);
	//	SetMoney(PlayerStateSave->Money);
	//}

	//UE_LOG(LogTemp, Log, TEXT("------Init----"));
	//UE_LOG(LogTemp, Log, TEXT("Total Ammo : %d, Current Ammo : %d"), TotalAmmo, CurrentAmmo);
	//UE_LOG(LogTemp, Log, TEXT("Flag  % d"), PlayerStateSaveFlag);
	//UE_LOG(LogTemp, Log, TEXT("--------------"));
}


bool AFPlayerState::GetWeaponEquipFlag()
{
	//FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {	
		return FGameInstance->WeaponEquipFlag;
	}

	return false;
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

	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
}

void AFPlayerState::SetCurrentAndTotalAmmo(int32 _CurrentAmmo, int32 _TotalAmmo)
{
	CurrentAmmo = FMath::Max(0.f, _CurrentAmmo);
	TotalAmmo = FMath::Max(0.f, _TotalAmmo);

	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
}

void AFPlayerState::SetWeaponEquipFlagOn()
{
	FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {
		FGameInstance->WeaponEquipFlag = true;
	}
}

void AFPlayerState::OnDeactivated()
{
	FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {
		FGameInstance->CurrentAmmo = CurrentAmmo;
		FGameInstance->TotalAmmo = TotalAmmo;
		UE_LOG(LogTemp, Log, TEXT("FGameInstance Ammo : %d"), FGameInstance->TotalAmmo);
		UE_LOG(LogTemp, Log, TEXT("FGameInstance CurrentAmmo : %d"), FGameInstance->CurrentAmmo);
	}
}





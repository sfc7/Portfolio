// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/MonsterComponent.h"
#include "Game/FGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

UMonsterComponent::UMonsterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMonsterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsDead);
	DOREPLIFETIME(ThisClass, bIsAttacking);
	DOREPLIFETIME(ThisClass, CurrentHp);
}

void UMonsterComponent::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GameInstance)) {
		if (nullptr != GameInstance->GetMonsterTable() || nullptr != GameInstance->GetMonsterTableRowFromName(MonsterName)) {
			float InstanceMaxHp = GameInstance->GetMonsterTableRowFromName(MonsterName)->MaxHp;
			SetMaxHp(InstanceMaxHp);	
			SetCurrentHp(InstanceMaxHp);
			float InstanceExpValue = GameInstance->GetMonsterTableRowFromName(MonsterName)->EXP;
			SetMonsterExpValue(InstanceExpValue);
;		}
	}
}

void UMonsterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMonsterComponent::SetMonsterName(FName _Name)
{
	MonsterName = _Name;
}

void UMonsterComponent::SetCurrentHp(float _CurrentHp)
{
	CurrentHp = FMath::Clamp(_CurrentHp, 0.f, MaxHp);

	if (CurrentHp < KINDA_SMALL_NUMBER) {
		CurrentHp = 0.f;
	}
}

void UMonsterComponent::SetMaxHp(float _MaxHp)
{
	MaxHp = _MaxHp;
}

void UMonsterComponent::SetMonsterExpValue(float _MonsterExpValue)
{
	MonsterExpValue = _MonsterExpValue;
}

void UMonsterComponent::SetIsAttacking(uint8 _bIsAttacking)
{
	bIsAttacking = _bIsAttacking;
}

void UMonsterComponent::SetIsDead(uint8 _bIsDead)
{
	bIsDead = _bIsDead;
}



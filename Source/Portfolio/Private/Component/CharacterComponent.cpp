// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CharacterComponent.h"
#include "Components/SphereComponent.h"
#include "Game/FGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Game/FPlayerState.h"
#include "Character/PlayerCharacter.h"
#include "WorldStatic/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/PlayerCharacter.h"

UCharacterComponent::UCharacterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MaxHp);
	DOREPLIFETIME(ThisClass, CurrentHp);
	DOREPLIFETIME(ThisClass, bIsAiming);
	DOREPLIFETIME(ThisClass, bIsDead);
	DOREPLIFETIME(ThisClass, CurrentState);
	DOREPLIFETIME(ThisClass, ReloadMaxAmmo);
	DOREPLIFETIME(ThisClass, TotalAmmo);
	DOREPLIFETIME(ThisClass, CurrentAmmo);
	DOREPLIFETIME(ThisClass, bWeaponEquipFlag);
}

void UCharacterComponent::InitCharacterComponent()
{
	FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {
		ReloadMaxAmmo = 0;
		TotalAmmo = FGameInstance->TotalAmmo;
		CurrentAmmo = FGameInstance->CurrentAmmo;
	}
}

void UCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	//InitCharacterComponent();

	FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(FGameInstance)) {
		if (nullptr != FGameInstance->GetCharacterTable() || nullptr != FGameInstance->GetCharacterTableRowFromLevel(1)) {
			float InstanceMaxHp = FGameInstance->GetCharacterTableRowFromLevel(1)->MaxHp;
			SetMaxHp(InstanceMaxHp);
			SetCurrentHp(InstanceMaxHp);
		}
	}

	PlayerCharacter = GetOwner<APlayerCharacter>();
	if (IsValid(PlayerCharacter)) {
		AFPlayerState* PlayerState = Cast<AFPlayerState>(PlayerCharacter->GetPlayerState());
		if (IsValid(PlayerState)) {
			if (!PlayerState->OnCurrentLevelChangedDelegate.IsAlreadyBound(this, &ThisClass::OnCurrentLevelChanged)) {
				PlayerState->OnCurrentLevelChangedDelegate.AddDynamic(this, &ThisClass::OnCurrentLevelChanged);
			}
		}

		if (PlayerCharacter->IsLocallyControlled()) {
			UFGameInstance* PlayerCharacterGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
			if (IsValid(PlayerCharacterGameInstance)) {
				SetCurrentAndTotalAmmo(PlayerCharacterGameInstance->CurrentAmmo, PlayerCharacterGameInstance->TotalAmmo);
				bWeaponEquipFlag = PlayerCharacterGameInstance->bWeaponEquipFlag;
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginPlay TotalAmmo : %d"), TotalAmmo), true, true, FLinearColor::Green, 10.0f);
				UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginPlay CurrentAmmo : %d"), CurrentAmmo), true, true, FLinearColor::Green, 10.0f);
			}
		}
	}
}

void UCharacterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCharacterComponent::SetCurrentHp(float _CurrentHp)
{
	if (OnCurrnetHpChangeDelegate.IsBound()) {
		OnCurrnetHpChangeDelegate.Broadcast(CurrentHp, _CurrentHp);
	}

	CurrentHp = FMath::Clamp(_CurrentHp, 0.f, MaxHp);

	if (_CurrentHp < KINDA_SMALL_NUMBER) {
		CurrentHp = 0.f;
		bIsDead = true;
	}

	OnCurrentHPChanged_NetMulticast(CurrentHp, CurrentHp);	
}

void UCharacterComponent::OnCurrentHPChanged_NetMulticast_Implementation(float _CurrentHp, float NewCurrntHp)
{
	if (OnCurrnetHpChangeDelegate.IsBound()) {
		OnCurrnetHpChangeDelegate.Broadcast(_CurrentHp, NewCurrntHp);	
	}
}

void UCharacterComponent::SetMaxHp(float _MaxHp)
{
	if (OnMaxHpChangeDelegate.IsBound()) {
		OnMaxHpChangeDelegate.Broadcast(MaxHp, _MaxHp);
	}

	MaxHp = _MaxHp;
}

void UCharacterComponent::SetCurrenState(ECurrentState _CurrentState)
{
	CurrentState = _CurrentState;
	SetCurrenState_Server(_CurrentState);
}

void UCharacterComponent::SetCurrenState_Server_Implementation(ECurrentState _CurrentState)
{
	CurrentState = _CurrentState;
}

void UCharacterComponent::EquipWeapon(AWeapon* _Weapon)
{
	if (PlayerCharacter == nullptr || _Weapon == nullptr) return;

	EquippedWeapon = _Weapon;
	EquippedWeapon->SetWeaponState(EWeaponState::Equip);
	FName WeaponSocketName = FName(TEXT("Weapon_Socket"));
	if (PlayerCharacter->GetMesh()->DoesSocketExist(WeaponSocketName)) {
		const USkeletalMeshSocket* WeaponSocket = PlayerCharacter->GetMesh()->GetSocketByName(FName("Weapon_Socket"));
		WeaponSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());
	}
}

void UCharacterComponent::SetReloadMaxAmmo(int32 _ReloadMaxAmmo)
{
	ReloadMaxAmmo = _ReloadMaxAmmo;
}

void UCharacterComponent::SetTotalAmmo(int32 _TotalAmmo)
{
	TotalAmmo = _TotalAmmo;

	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
}

void UCharacterComponent::SetCurrentAmmo(int32 _CurrentAmmo)
{
	if (_CurrentAmmo >= 0) {
		CurrentAmmo = _CurrentAmmo;
	}

	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
}

void UCharacterComponent::SetCurrentAndTotalAmmo(int32 _CurrentAmmo, int32 _TotalAmmo)
{
	CurrentAmmo = FMath::Max(0.f, _CurrentAmmo);
	TotalAmmo = FMath::Max(0.f, _TotalAmmo);

	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
}

void UCharacterComponent::SetWeaponEquipFlag()
{
	bWeaponEquipFlag = true;
	if (IsValid(FGameInstance)) {
		FGameInstance->bWeaponEquipFlag = true;
	}
}

void UCharacterComponent::SetAiming(bool _bIsAiming)
{
	bIsAiming = _bIsAiming;
	SetAiming_Server(_bIsAiming);
}

void UCharacterComponent::SetAiming_Server_Implementation(bool _bIsAiming)
{
	bIsAiming = _bIsAiming;
}

void UCharacterComponent::OnCurrentLevelChanged(int32 NewCurrentLevel)
{
	SetMaxHp(FGameInstance->GetCharacterTableRowFromLevel(NewCurrentLevel)->MaxHp);
	SetCurrentHp(FGameInstance->GetCharacterTableRowFromLevel(NewCurrentLevel)->MaxHp);
}







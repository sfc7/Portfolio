// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CharacterComponent.h"
#include "Components/SphereComponent.h"
#include "Game/FGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Data/DataStruct.h"
#include "Game/FPlayerState.h"
#include "Character/PlayerCharacter.h"
#include "WorldStatic/Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/PlayerCharacter.h"
#include "Controller/PlayerCharacterController.h"
#include "UI/AimingHUD.h"
#include "WorldStatic/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UCharacterComponent::UCharacterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<AWeapon> RifleBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Actor/Weapon/BP_Rifle.BP_Rifle_C'"));

	if (RifleBP.Succeeded()) {
		DefaultWeaponType = RifleBP.Class;
	}
}

void UCharacterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MaxHp);
	DOREPLIFETIME(ThisClass, CurrentHp);
	DOREPLIFETIME(ThisClass, bIsAiming);
	DOREPLIFETIME(ThisClass, bIsDead);
	DOREPLIFETIME(ThisClass, CurrentState);
	DOREPLIFETIME(ThisClass, CurrentWeaponType);
	DOREPLIFETIME(ThisClass, DefaultWeaponType);

	DOREPLIFETIME_CONDITION(ThisClass, EquippedWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, ReloadMaxAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, TotalAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentLevel, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, MaxEXP, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentEXP, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, PlayerMoney, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, MaxLevel, COND_OwnerOnly);
}

void UCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

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
		if (IsValid(FGameInstance)) {
			if (PlayerCharacter->IsLocallyControlled() || (PlayerCharacter->HasAuthority())) {
				CurrentAmmo = FGameInstance->CurrentAmmo;
				TotalAmmo = FGameInstance->TotalAmmo;
				ReloadMaxAmmo = FGameInstance->ReloadMaxAmmo;
				if (FGameInstance->CurrentWeaponType != nullptr) {
					CurrentWeaponType = FGameInstance->CurrentWeaponType;
				}

				SendGameInstanceWeaponVariable_Server(TotalAmmo, CurrentAmmo, ReloadMaxAmmo, CurrentWeaponType);

				if (nullptr != FGameInstance->GetCharacterTable() || nullptr != FGameInstance->GetCharacterTableRowFromLevel(1)) {
					MaxLevel = FGameInstance->GetCharacterTable()->GetRowMap().Num();
					CurrentLevel = FGameInstance->CurrentLevel;
					CurrentEXP = FGameInstance->CurrentEXP;
					MaxEXP = FGameInstance->GetCharacterTableRowFromLevel(CurrentLevel)->MaxEXP;
					/*PlayerMoney = FGameInstance->PlayerMoney;*/
					PlayerMoney = 300000;
				}

				SendGameInstanceXPVariable_Server(CurrentLevel, CurrentEXP, PlayerMoney);
			}
		}
	}
}

void UCharacterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetHUDCrossHair(DeltaTime);
}

void UCharacterComponent::SetCurrentLevel(int32 _CurrentLevel)
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

void UCharacterComponent::SetCurrentEXP(float _CurrentEXP)
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

void UCharacterComponent::SetMoney(int32 _Money)
{
	PlayerMoney = _Money;

	OnMoneyChangeDelegate.Broadcast(_Money);
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

void UCharacterComponent::SetHUDCrossHair(float DeltaTime)
{
	if (PlayerCharacter == nullptr) {
		return;
	}

	if (PlayerCharacter->GetController() == nullptr) {
		return;
	}


	PlayerCharacterController = Cast<APlayerCharacterController>(PlayerCharacter->GetController());
	if (IsValid(PlayerCharacterController)) {
		AimingHUD = Cast<AAimingHUD>(PlayerCharacterController->GetHUD());
		if (AimingHUD) {
			if (EquippedWeapon) {
				CrossHairTexture.CrossHairCenter = EquippedWeapon->CrossHairCenter;
				CrossHairTexture.CrossHairLeft = EquippedWeapon->CrossHairLeft;
				CrossHairTexture.CrossHairRight = EquippedWeapon->CrossHairRight;
				CrossHairTexture.CrossHairTop = EquippedWeapon->CrossHairTop;
				CrossHairTexture.CrossHairBottom = EquippedWeapon->CrossHairBottom;
			}
			else {
				CrossHairTexture.CrossHairCenter = nullptr;
				CrossHairTexture.CrossHairLeft = nullptr;
				CrossHairTexture.CrossHairRight = nullptr;
				CrossHairTexture.CrossHairTop = nullptr;
				CrossHairTexture.CrossHairBottom = nullptr;
			}

			FVector2D CharacterWalkSpeedRange(0.f, PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D ClampedValue(0.f, 1.f);
			FVector Velocity = PlayerCharacter->GetVelocity();
			Velocity.Z = 0.f;

			CrossHairVelocityValue = FMath::GetMappedRangeValueClamped(CharacterWalkSpeedRange, ClampedValue, Velocity.Size());

			if (PlayerCharacter->GetCharacterMovement()->IsFalling()) {
				CrossHairJumpValue = FMath::FInterpTo(CrossHairJumpValue, 1.5f, DeltaTime, 1.5f);
			}
			else {
				CrossHairJumpValue = FMath::FInterpTo(CrossHairJumpValue, 0.f, DeltaTime, 30.f);
			}

			if (bIsAiming) {
				CrossHairAimingValue = FMath::FInterpTo(CrossHairAimingValue, 0.58f, DeltaTime, 30.f);
			}
			else {
				CrossHairAimingValue = FMath::FInterpTo(CrossHairAimingValue, 0.f, DeltaTime, 30.f);
			}

			CrossHairShootingValue = FMath::FInterpTo(CrossHairShootingValue, 0.f, DeltaTime, 10.f);
			CrossHairTexture.CrossHairSpread = 0.5f + CrossHairVelocityValue + CrossHairJumpValue - CrossHairAimingValue + CrossHairShootingValue;


			AimingHUD->SetCrossHairTexture(CrossHairTexture);
		}
	}
}

void UCharacterComponent::SetCurrenState_Server_Implementation(ECurrentState _CurrentState)
{
	CurrentState = _CurrentState;
}

void UCharacterComponent::EquipWeapon(AWeapon* _Weapon)
{
	if (PlayerCharacter == nullptr || _Weapon == nullptr) return;

	EquippedWeapon = _Weapon;

	if (GetCurrentWeaponType() == nullptr) {
		SetCurrentAndTotalAmmo(EquippedWeapon->GetReloadMaxAmmo(), EquippedWeapon->GetTotalAmmo());
		SetReloadMaxAmmo(EquippedWeapon->GetReloadMaxAmmo());
		SetCurrentWeaponType(GetDefaultWeaponType());
	}
	else {
		SetCurrentWeaponType(EquippedWeapon->GetClass());
	}

	
	//FName WeaponSocketName = FName(TEXT("Weapon_Socket"));
	//if (PlayerCharacter->GetMesh()->DoesSocketExist(WeaponSocketName)) {
	//	const USkeletalMeshSocket* WeaponSocket = PlayerCharacter->GetMesh()->GetSocketByName(FName("Weapon_Socket"));
	//	WeaponSocket->AttachActor(EquippedWeapon, PlayerCharacter->GetMesh());
	//}
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

void UCharacterComponent::SetCurrentWeaponType(TSubclassOf<class AWeapon> _CurrentWeaponType)
{
	CurrentWeaponType = _CurrentWeaponType;
}

void UCharacterComponent::SetDefaultWeaponType(TSubclassOf<class AWeapon> _DefaultWeaponType)
{
	DefaultWeaponType = _DefaultWeaponType;
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

void UCharacterComponent::OnRep_CurrentWeapon()
{
	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
	
}

void UCharacterComponent::OnRep_TotalWeapon()
{	
	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
}

void UCharacterComponent::OnRep_CurrentEXP()
{
	OnCurrentEXPChangedDelegate.Broadcast(CurrentEXP);
}

void UCharacterComponent::OnRep_MaxEXP()
{
	OnMaxEXPChangedDelegate.Broadcast(MaxEXP);
}

void UCharacterComponent::OnRep_PlayerMoney()
{
	OnMoneyChangeDelegate.Broadcast(PlayerMoney);
}

void UCharacterComponent::OnRep_CurrentLevel()
{
	OnCurrentLevelChangedDelegate.Broadcast(CurrentLevel);
}

void UCharacterComponent::SendGameInstanceWeaponVariable_Server_Implementation(int32 _TotalAmmo, int32 _CurrentAmmo, int32 _ReloadMaxAmmo, TSubclassOf<AWeapon> _CurrentWeaponType)
{

	TotalAmmo = _TotalAmmo;
	CurrentAmmo = _CurrentAmmo;
	ReloadMaxAmmo = _ReloadMaxAmmo;
	CurrentWeaponType = _CurrentWeaponType;
}

void UCharacterComponent::SendGameInstanceXPVariable_Server_Implementation(int32 _CurrentLevel, int32 _CurrentEXP, int32 _Money)
{
	CurrentLevel = _CurrentLevel;
	CurrentEXP = _CurrentEXP;
	PlayerMoney = _Money;
}

void UCharacterComponent::SetCrossHairColor_Red()
{
	CrossHairTexture.CrossHairColor = FLinearColor::Red;
	FTimerHandle CrossHairColorHandle;

	if(GetWorld()->GetTimerManager().IsTimerActive(CrossHairColorHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(CrossHairColorHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(CrossHairColorHandle, FTimerDelegate::CreateLambda([this]() {
		CrossHairTexture.CrossHairColor = FLinearColor::White;
	}), 2.0f, false);

}
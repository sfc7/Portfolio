// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Component/CharacterComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Portfolio/Portfolio.h"
#include "Controller/PlayerCharacterController.h"
#include "Game/PlayerStateSave.h"
#include "Game/FPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "WorldStatic/Weapon/Weapon.h"
#include "Portfolio/Portfolio.h"
#include "Game/FGameInstance.h"
#include "UI/PlayerHUD.h"


// Sets default values
AFCharacter::AFCharacter()
{	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	FVector PivotPosition(0.f, 0.f, -88.0f);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	CharacterComponent = CreateDefaultSubobject<UCharacterComponent>(TEXT("CharacterComponent"));
	CharacterComponent->SetIsReplicated(true);
}

void AFCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled()) {
		FPlayerState = GetPlayerState<AFPlayerState>();
		UPlayerStateSave* PlayerStateLoad = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));

		if (HasAuthority()) {
			if (IsValid(PlayerStateLoad)) {
				EquipWeapon();
			}
			else {
				FirstEquipWeapon();
			}

			LoadPlayerStateSave();
			WeaponHudBind();
		}
		else {
			if (IsValid(PlayerStateLoad)) {
				EquipWeapon_Server();
			}
			else {
				FirstEquipWeapon_Server();
			}
		}
	}
}

void AFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AFCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AFCharacter::LoadPlayerStateSave()
{
	if (IsLocallyControlled()) {
		UPlayerStateSave* PlayerStateLoad = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));
		if (IsValid(PlayerStateLoad) && !bFirstPlayerStateLoad) {
			WeaponSlot.FirstPrimaryWeapon->SetWeaponData(PlayerStateLoad->WeaponSlotSaveData.FirstPrimaryWeaponData);
			WeaponSlot.SecondPrimaryWeapon->SetWeaponData(PlayerStateLoad->WeaponSlotSaveData.SecondPrimaryWeaponData);
			WeaponSlot.FirstPrimaryWeapon->SetWeaponMesh(PlayerStateLoad->WeaponSlotSaveData.FirstPrimaryWeaponData.Mesh);
			WeaponSlot.SecondPrimaryWeapon->SetWeaponMesh(PlayerStateLoad->WeaponSlotSaveData.SecondPrimaryWeaponData.Mesh);

			bFirstPlayerStateLoad = true;
		}
	}
}

void AFCharacter::WeaponHudBind()
{
	APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController());
	if (IsValid(PlayerController)) {
		if (IsValid(CharacterComponent)) {
			PlayerController->GetHUDWidget()->BindWeapoon(WeaponSlot.FirstPrimaryWeapon);
			PlayerController->GetHUDWidget()->BindWeapoon(WeaponSlot.SecondPrimaryWeapon);
			PlayerController->GetHUDWidget()->BindCharacterComponent(CharacterComponent);
			if (CurrentWeapon) {
				CurrentWeapon->SetCurrentAndTotalAmmo(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetTotalAmmo());
			}
		}
	}
}

void AFCharacter::OnRep_Weapon()
{
	if (IsLocallyControlled()) {
		LoadPlayerStateSave();
		WeaponHudBind();
	}
}

void AFCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicateMesh);
	DOREPLIFETIME(ThisClass, CurrentWeapon);
	DOREPLIFETIME(ThisClass, WeaponSlot);
}

void AFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AFCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	return ActualDamage;
}

void AFCharacter::SetSkeletalMeshInPlayerStateSave(USkeletalMesh* _PlayerMesh)
{
	UPlayerStateSave* PlayerStateLoad = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));
	if (!IsValid(PlayerStateLoad)) {
		PlayerStateLoad = NewObject<UPlayerStateSave>();
		PlayerStateLoad->PlayerMesh = _PlayerMesh;
	}
	else {
		PlayerStateLoad->PlayerMesh = _PlayerMesh;
	}

	UGameplayStatics::SaveGameToSlot(PlayerStateLoad, FString::FromInt(GPlayInEditorID), 0);

	SetPlayerMesh_Server(_PlayerMesh);
}

void AFCharacter::SetPlayerMesh_Server_Implementation(USkeletalMesh* _PlayerMesh)
{
	ReplicateMesh = _PlayerMesh; 
	GetMesh()->SetSkeletalMesh(ReplicateMesh);
}

void AFCharacter::OnRep_Mesh()
{
	GetMesh()->SetSkeletalMesh(ReplicateMesh);
}

void AFCharacter::EquipWeapon()
{
	WeaponSlot.FirstPrimaryWeapon = GetWorld()->SpawnActor<AWeapon>(AWeapon::StaticClass());
	if (IsValid(WeaponSlot.FirstPrimaryWeapon)) {
		WeaponSlot.FirstPrimaryWeapon->SetOwner(this);
		WeaponAttachToSocket(WeaponSlot.FirstPrimaryWeapon);
		WeaponSlot.FirstPrimaryWeapon->SetWeaponState(EWeaponState::Equipped);
		CurrentWeapon = WeaponSlot.FirstPrimaryWeapon;
	}
	WeaponSlot.SecondPrimaryWeapon = GetWorld()->SpawnActor<AWeapon>(AWeapon::StaticClass());
	if (IsValid(WeaponSlot.SecondPrimaryWeapon)) {
		WeaponSlot.SecondPrimaryWeapon->SetOwner(this);
		WeaponSlot.SecondPrimaryWeapon->SetWeaponState(EWeaponState::UnEquipped);
		WeaponAttachToSocket(WeaponSlot.SecondPrimaryWeapon);
	}
}

void AFCharacter::FirstEquipWeapon()
{
	WeaponSlot.FirstPrimaryWeapon = GetWorld()->SpawnActor<AWeapon>(GetCharacterComponent()->GetDefaultFirstPrimaryWeaponType());
	if (IsValid(WeaponSlot.FirstPrimaryWeapon)) {
		WeaponSlot.FirstPrimaryWeapon->SetOwner(this);
		WeaponSlot.FirstPrimaryWeapon->SetWeaponState(EWeaponState::Equipped);
		WeaponAttachToSocket(WeaponSlot.FirstPrimaryWeapon);
		CurrentWeapon = WeaponSlot.FirstPrimaryWeapon;
	}
	WeaponSlot.SecondPrimaryWeapon = GetWorld()->SpawnActor<AWeapon>(GetCharacterComponent()->GetDefaultSecondPrimaryWeaponType());
	if (IsValid(WeaponSlot.SecondPrimaryWeapon)) {
		WeaponSlot.SecondPrimaryWeapon->SetOwner(this);
		WeaponSlot.SecondPrimaryWeapon->SetWeaponState(EWeaponState::UnEquipped);
		WeaponAttachToSocket(WeaponSlot.SecondPrimaryWeapon);
	}
}

void AFCharacter::WeaponAttachToSocket(AWeapon* _Weapon)
{
	if (_Weapon->GetWeaponData().WeaponType == EWeaponType::AR) {
		FName WeaponSocketName(TEXT("AR_Socket"));
		if (GetMesh()->DoesSocketExist(WeaponSocketName)) {
			_Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
		}
	}
	else if (_Weapon->GetWeaponData().WeaponType == EWeaponType::SG) {
		FName WeaponSocketName(TEXT("SG_Socket"));
		if (GetMesh()->DoesSocketExist(WeaponSocketName)) {
			_Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
		}
	}
}

void AFCharacter::EquipWeapon_Server_Implementation()
{
	EquipWeapon();
}

void AFCharacter::FirstEquipWeapon_Server_Implementation()
{
	FirstEquipWeapon();
}
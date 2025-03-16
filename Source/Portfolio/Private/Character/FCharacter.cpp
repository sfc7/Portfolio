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
			WeaponSlot.FirstPrimaryWeapon->SetWeaponMesh(PlayerStateLoad->WeaponSlotSaveData.FirstPrimaryWeaponData.Mesh);
			WeaponSlot.SecondPrimaryWeapon->SetWeaponData(PlayerStateLoad->WeaponSlotSaveData.SecondPrimaryWeaponData);
			WeaponSlot.SecondPrimaryWeapon->SetWeaponMesh(PlayerStateLoad->WeaponSlotSaveData.SecondPrimaryWeaponData.Mesh);
			WeaponAttachToSocket_Server();

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
				CurrentWeapon->SetCurrentAndTotalAmmo(CurrentWeapon->GetWeaponData().CurrentAmmo, CurrentWeapon->GetWeaponData().TotalAmmo);
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
	FString SlotName = TEXT("Playermesh") + FString::FromInt(GPlayInEditorID);
	UPlayerStateSave* PlayerMeshLoad = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!IsValid(PlayerMeshLoad)) {
		PlayerMeshLoad = NewObject<UPlayerStateSave>();
		PlayerMeshLoad->PlayerMesh = _PlayerMesh;
	}
	else {
		PlayerMeshLoad->PlayerMesh = _PlayerMesh;
	}


	UGameplayStatics::SaveGameToSlot(PlayerMeshLoad, SlotName, 0);

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
		WeaponSlot.FirstPrimaryWeapon->SetWeaponState(EWeaponState::Equipped);
		CurrentWeapon = WeaponSlot.FirstPrimaryWeapon;
	}
	WeaponSlot.SecondPrimaryWeapon = GetWorld()->SpawnActor<AWeapon>(AWeapon::StaticClass());
	if (IsValid(WeaponSlot.SecondPrimaryWeapon)) {
		WeaponSlot.SecondPrimaryWeapon->SetOwner(this);
		WeaponSlot.SecondPrimaryWeapon->SetWeaponState(EWeaponState::UnEquipped);
		WeaponSlot.SecondPrimaryWeapon->CrossHairLeft = nullptr;
		WeaponSlot.SecondPrimaryWeapon->CrossHairRight = nullptr;
		WeaponSlot.SecondPrimaryWeapon->CrossHairTop = nullptr;
		WeaponSlot.SecondPrimaryWeapon->CrossHairBottom = nullptr;
		WeaponSlot.SecondPrimaryWeapon->CrossHairCenter = WeaponSlot.SecondPrimaryWeapon->CrossHairSG;
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
	if (_Weapon->GetWeaponData().WeaponType == EWeaponType::AR || _Weapon->GetWeaponData().WeaponType == EWeaponType::DMR) {
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
	else if (_Weapon->GetWeaponData().WeaponType == EWeaponType::SMG) {
		FName WeaponSocketName(TEXT("SMG_Socket"));
		if (GetMesh()->DoesSocketExist(WeaponSocketName)) {
			_Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
		}
	}
}

void AFCharacter::WeaponAttachToSocket_Server_Implementation()
{
	WeaponAttachToSocket(WeaponSlot.FirstPrimaryWeapon);
	WeaponAttachToSocket(WeaponSlot.SecondPrimaryWeapon);
}

void AFCharacter::EquipWeapon_Server_Implementation()
{
	EquipWeapon();
}

void AFCharacter::FirstEquipWeapon_Server_Implementation()
{
	FirstEquipWeapon();
}
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
#include "WorldStatic/Weapon.h"
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

		APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController());
		if (IsValid(PlayerController)) {
			if (IsValid(CharacterComponent)) {
				PlayerController->GetHUDWidget()->BindCharacterComponent(CharacterComponent);
			}
		}

		if (HasAuthority()) {
			EquipWeapon();
			WeaponSetCharacterComponentOnStart();
		}
		else {
			EquipWeapon_Server();
			WeaponSetCharacterComponentOnStart_Server();
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

void AFCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicateMesh);
	DOREPLIFETIME(ThisClass, Weapon);
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
	SetPlayerMesh_Multicast(ReplicateMesh); // 클라의 캐릭터 모두에게 동기화
}

void AFCharacter::SetPlayerMesh_Multicast_Implementation(USkeletalMesh* _PlayerMesh)
{
	GetMesh()->SetSkeletalMesh(_PlayerMesh);
}

void AFCharacter::SetPlayerMesh_Client_Implementation(USkeletalMesh* _PlayerMesh)
{
	GetMesh()->SetSkeletalMesh(_PlayerMesh);
}

void AFCharacter::OnRep_Mesh()
{
	if (!HasAuthority())
	{
		SetPlayerMesh_Client(ReplicateMesh); // 클라에서 서버클라의 캐릭터 동기화
	}
	else {
		GetMesh()->SetSkeletalMesh(ReplicateMesh); // 서버의 캐릭터 setskeletalmsh
	}
}

void AFCharacter::WeaponSetCharacterComponentOnStart()
{
	if (IsValid(Weapon) && IsValid(GetCharacterComponent()) && GetCharacterComponent()->GetCurrentWeaponType() == nullptr) {
		GetCharacterComponent()->SetCurrentWeaponType(GetCharacterComponent()->GetDefaultWeaponType());
		GetCharacterComponent()->EquipWeapon(Weapon);
		GetCharacterComponent()->SetCurrentAndTotalAmmo(Weapon->GetReloadMaxAmmo(), Weapon->GetTotalAmmo());
		GetCharacterComponent()->SetReloadMaxAmmo(Weapon->GetReloadMaxAmmo());
	}
}

void AFCharacter::WeaponSetCharacterComponentOnStart_Server_Implementation()
{
	WeaponSetCharacterComponentOnStart();
}

void AFCharacter::EquipWeapon()
{

	if (IsValid(GetCharacterComponent()) && GetCharacterComponent()->GetCurrentWeaponType() == nullptr) {
		Weapon = GetWorld()->SpawnActor<AWeapon>(GetCharacterComponent()->GetDefaultWeaponType());
	}
	else if (IsValid(GetCharacterComponent()) && IsValid(GetCharacterComponent()->GetCurrentWeaponType())) {
		Weapon = GetWorld()->SpawnActor<AWeapon>(GetCharacterComponent()->GetCurrentWeaponType());
	}

	if (IsValid(Weapon)) {
		FName WeaponSocketName = FName(TEXT("Weapon_Socket"));
		if (GetMesh()->DoesSocketExist(WeaponSocketName)) {
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
			Weapon->SetOwner(this);
			Weapon->ShowPickUpText(false);
		}
	}
}

void AFCharacter::EquipWeapon_Server_Implementation()
{
	EquipWeapon();
}

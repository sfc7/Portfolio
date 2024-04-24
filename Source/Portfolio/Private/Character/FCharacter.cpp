// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Portfolio/Portfolio.h"
#include "Component/CharacterComponent.h"
#include "Game/PlayerStateSave.h"
#include "Game/FPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "WorldStatic/Weapon.h"
#include "Portfolio/Portfolio.h"

// Sets default values
AFCharacter::AFCharacter()
{	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FVector PivotPosition(0.f, 0.f, -88.0f);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	CharacterComponent = CreateDefaultSubobject<UCharacterComponent>(TEXT("CharacterComponent"));
	CharacterComponent->SetIsReplicated(true);

	static ConstructorHelpers::FClassFinder<AWeapon> RifleBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Actor/Weapon/BP_Rifle.BP_Rifle_C'"));
	if (RifleBP.Succeeded()) {
		Rifle = RifleBP.Class;
	}
}

void AFCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FPlayerState = GetPlayerState<AFPlayerState>();
	if (IsValid(FPlayerState)) {
		UPlayerStateSave* PlayerStateSave = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));

		//if (IsLocallyControlled()) {
		//	SetPlayerMesh_Server(PlayerStateSave->PlayerMesh);
		//}
	}
	else {
		USER_LOG(LogUser, Log, TEXT("cant initialize"));
	}

	EquipWeapon();
}

void AFCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	FPlayerState = GetPlayerState<AFPlayerState>();
	if (IsValid(FPlayerState)) {
		UPlayerStateSave* PlayerStateSave = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));

		//if (IsLocallyControlled()) {
		//	SetPlayerMesh_Server(PlayerStateSave->PlayerMesh);
		//}
	}
	else {
		USER_LOG(LogUser, Log, TEXT("cant initialize"));
	}

	EquipWeapon();
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
	SetPlayerMesh_Multicast(ReplicateMesh); // Ŭ���� ĳ���� ��ο��� ����ȭ
}

void AFCharacter::SetPlayerMesh_Multicast_Implementation(USkeletalMesh* _PlayerMesh)
{
	GetMesh()->SetSkeletalMesh(_PlayerMesh);
}

void AFCharacter::SetPlayerMesh_Client_Implementation(USkeletalMesh* _PlayerMesh)
{
		GetMesh()->SetSkeletalMesh(_PlayerMesh);
	}

void AFCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicateMesh);
}

void AFCharacter::OnRep_Mesh()
{
	if (!HasAuthority())
	{
		SetPlayerMesh_Client(ReplicateMesh); // Ŭ�󿡼� ����Ŭ���� ĳ���� ����ȭ
	}
	else {
		GetMesh()->SetSkeletalMesh(ReplicateMesh); // ������ ĳ���� setskeletalmsh
	}
}

void AFCharacter::EquipWeapon()
{
	if (Rifle) {
		Weapon = GetWorld()->SpawnActor<AWeapon>(Rifle);
		if (IsValid(Weapon)) {
			FName WeaponSocketName = FName(TEXT("Weapon_Socket"));
			if (GetMesh()->DoesSocketExist(WeaponSocketName)) {
				Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
				Weapon->SetOwner(this);
				Weapon->ShowPickUpText(false);
				if (GetCharacterComponent()) {
					GetCharacterComponent()->EquipWeapon(Weapon);
				}
				if (GetPlayerState()) {
					if (IsValid(FPlayerState)) {
						USER_LOG(LogUser, Log, TEXT("Ammo : %d"), Weapon->GetReloadMaxAmmo());
						FPlayerState->SetCurrentAmmo(Weapon->GetReloadMaxAmmo());
						FPlayerState->SetReloadMaxAmmo(Weapon->GetReloadMaxAmmo());
						FPlayerState->SetTotalAmmo(Weapon->GetTotalAmmo());
					}
				}
			}
		}
	}
}



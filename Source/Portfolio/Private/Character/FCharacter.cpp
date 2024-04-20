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

	static ConstructorHelpers::FClassFinder<AWeapon> RifleBP(TEXT("/Script/Engine.Blueprint'/Game/Source/Actor/Weapon/BP_Weapon.BP_Weapon_C'"));
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

void AFCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicateMesh);
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
					AFPlayerState* PlayerState = Cast<AFPlayerState>(GetPlayerState());
					if (IsValid(PlayerState)) {
						PlayerState->SetCurrentAmmo(Weapon->GetReloadMaxAmmo());
						PlayerState->SetReloadMaxAmmo(Weapon->GetReloadMaxAmmo());
						PlayerState->SetTotalAmmo(Weapon->GetTotalAmmo());
					}
				}
			}
		}
	}
}



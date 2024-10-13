// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/Weapon/Weapon.h"
#include "Components/WidgetComponent.h"
#include "Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);    
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	InstanceInteractableData.Name = FText::FromString(WeaponName);
	InstanceInteractableData.InteractableType = EInteractableType::Trade;

	InteractableData = InstanceInteractableData; // InteractableData 와 InstanceInteractableData를 분리해서 만든 이유 : InstanceInteractableData는 기본 값, InteractableData는 현재값으로 분리하여 초기 상태로 복구 가능하게끔

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWeapon::BeginFocus()
{
	if (WeaponMesh) {
		WeaponMesh->SetRenderCustomDepth(true);
	}
}

void AWeapon::EndFoucs()
{
	if (WeaponMesh) {
		WeaponMesh->SetRenderCustomDepth(false);
	}
}

void AWeapon::BeginInteract()
{
}

void AWeapon::EndInteract()
{
}

void AWeapon::Interact()
{
	Destroy();
}

void AWeapon::SpawnMuzzleFlash_Server_Implementation()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("Muzzle"));
	SpawnMuzzleFlash_NetMulticast();
}

void AWeapon::SpawnMuzzleFlash_NetMulticast_Implementation()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("Muzzle"));
}


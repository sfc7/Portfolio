// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/Weapon/Weapon.h"
#include "Components/SphereComponent.h"
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

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpText = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpText"));
	PickUpText->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickUpText) {
		PickUpText->SetVisibility(false);
	}

	if (HasAuthority()) {
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereOverlapEnd);
	}

	InteractableData = InstanceInteractableData;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter) {
		PlayerCharacter->SetOverlapWeapon(this);
	}
}

void AWeapon::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter) {
		PlayerCharacter->SetOverlapWeapon(nullptr);
	}
}

void AWeapon::SetWeaponState(EWeaponState _WeaponState)
{
	WeaponState = _WeaponState;
	switch (WeaponState) {
	case EWeaponState::Equip:	
		ShowPickUpText(false);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}

}
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState) {
	case EWeaponState::Equip:
		ShowPickUpText(false);
		break;
	}
}

void AWeapon::ShowPickUpText(bool ShowFlag)
{
	if (PickUpText) {
		PickUpText->SetVisibility(ShowFlag);
	}
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
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("BeginInteract")));
}

void AWeapon::EndInteract()
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndInteract")));
}

void AWeapon::Interact()
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Interact")));
}

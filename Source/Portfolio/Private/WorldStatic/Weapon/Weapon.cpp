// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/Weapon/Weapon.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Portfolio/Portfolio.h"
#include "Game/FGameInstance.h"
#include "Game/PlayerStateSave.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);    
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrossHairCenterBP(TEXT("/Script/Engine.Texture2D'/Game/Source/Texture/UI/CrossHair/Crosshair_Center.Crosshair_Center'"));
	if (CrossHairCenterBP.Succeeded()) {
		CrossHairCenter = CrossHairCenterBP.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrossHairLeftBP(TEXT("/Script/Engine.Texture2D'/Game/Source/Texture/UI/CrossHair/Crosshair_Left.Crosshair_Left'"));
	if (CrossHairLeftBP.Succeeded()) {
		CrossHairLeft = CrossHairLeftBP.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrossHairRightBP(TEXT("/Script/Engine.Texture2D'/Game/Source/Texture/UI/CrossHair/Crosshair_Right.Crosshair_Right'"));
	if (CrossHairRightBP.Succeeded()) {
		CrossHairRight = CrossHairRightBP.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrossHairTopBP(TEXT("/Script/Engine.Texture2D'/Game/Source/Texture/UI/CrossHair/Crosshair_Top.Crosshair_Top'"));
	if (CrossHairTopBP.Succeeded()) {
		CrossHairTop = CrossHairTopBP.Object;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrossHairBottomBP(TEXT("/Script/Engine.Texture2D'/Game/Source/Texture/UI/CrossHair/Crosshair_Bottom.Crosshair_Bottom'"));
	if (CrossHairBottomBP.Succeeded()) {
		CrossHairBottom = CrossHairBottomBP.Object;
	}
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	InstanceInteractableData.Name = FText::FromString(WeaponName);
	InstanceInteractableData.InteractableType = EInteractableType::Trade;

	InteractableData = InstanceInteractableData; // InteractableData 와 InstanceInteractableData를 분리해서 만든 이유 : InstanceInteractableData는 기본 값, InteractableData는 현재값으로 분리하여 초기 상태로 복구 가능하게끔

	if (IsValid(WeaponMesh)) {
		WeaponData.Mesh = WeaponMesh->GetSkeletalMeshAsset();
	}
	

	//UFGameInstance* FGameInstance = Cast<UFGameInstance>(GetWorld()->GetGameInstance());
	//if (IsValid(FGameInstance)) {
	//	if (FGameInstance->WeaponData.CurrentAmmo != 0) {
	//		WeaponData = FGameInstance->WeaponData;
	//	}
	//	else {
	//		WeaponData.CurrentAmmo = 40;
	//	}
	//}

	

	//AFCharacter* CharacterOwner = Cast<AFCharacter>(GetOwner());
	//if (IsValid(CharacterOwner)) {
	//	USER_LOG(LogUser, Log, TEXT("dd"));
	//	if (CharacterOwner->HasAuthority()) {
	//		UPlayerStateSave* PlayerStateLoad = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));
	//		if (IsValid(PlayerStateLoad)) {
	//			SetWeaponData(PlayerStateLoad->WeaponData);
	//			UE_LOG(LogTemp, Log, TEXT("PlayerStateLoad O FGameInstance %d : %d"), GPlayInEditorID, GetWeaponData().CurrentAmmo);
	//		}		
	//		UGameplayStatics::SaveGameToSlot(PlayerStateLoad, FString::FromInt(GPlayInEditorID), 0);
	//		
	//	}
	//	else if(CharacterOwner->IsLocallyControlled()){
	//		UPlayerStateSave* PlayerStateLoad = Cast<UPlayerStateSave>(UGameplayStatics::LoadGameFromSlot(FString::FromInt(GPlayInEditorID), 0));
	//		if (IsValid(PlayerStateLoad)) {
	//			SetWeaponData(PlayerStateLoad->WeaponData);
	//			UE_LOG(LogTemp, Log, TEXT("PlayerStateLoad O FGameInstance %d : %d"), GPlayInEditorID, GetWeaponData().CurrentAmmo);
	//		}
	//		UGameplayStatics::SaveGameToSlot(PlayerStateLoad, FString::FromInt(GPlayInEditorID), 0);
	//		UE_LOG(LogTemp, Log, TEXT("PlayerStateLoad X FGameInstance %d : %d"), GPlayInEditorID, GetWeaponData().CurrentAmmo);
	//	}

	//}
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WeaponState);
	DOREPLIFETIME(ThisClass, ReplicateMesh);
	DOREPLIFETIME(ThisClass, WeaponMesh);
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

void AWeapon::SetWeaponMesh(USkeletalMesh* _Mesh)
{
	SetWeaponMesh_Server(_Mesh);
}

void AWeapon::SetWeaponMesh_Server_Implementation(USkeletalMesh* _Mesh)
{
	ReplicateMesh = _Mesh;
	WeaponData.Mesh = ReplicateMesh;
	WeaponMesh->SetSkeletalMesh(ReplicateMesh);
}

void AWeapon::OnRep_WeaponMesh()
{
	WeaponData.Mesh = ReplicateMesh;
	WeaponMesh->SetSkeletalMesh(ReplicateMesh);
}

void AWeapon::SetWeaponState(EWeaponState _State)
{
	WeaponState = _State;
	switch (WeaponState)
	{
	case EWeaponState::Equipped:
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		break;
	case EWeaponState::UnEquipped:
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		break;
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::Equipped:
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		break;
	case EWeaponState::UnEquipped:
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		break;
	}
}

void AWeapon::SetReloadMaxAmmo(int32 _ReloadMaxAmmo)
{
	WeaponData.ReloadMaxAmmo = _ReloadMaxAmmo;
}

void AWeapon::SetTotalAmmo(int32 _TotalAmmo)
{
	WeaponData.TotalAmmo = FMath::Max(0.f, _TotalAmmo);

	//OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
}

void AWeapon::SetWeaponData(FWeaponData _WeaponData)
{
	WeaponData = _WeaponData;

	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(WeaponData.CurrentAmmo, WeaponData.TotalAmmo);
}

void AWeapon::SetCurrentAmmo(int32 _CurrentAmmo)
{
	WeaponData.CurrentAmmo = FMath::Max(0.f, _CurrentAmmo);

	UE_LOG(LogTemp, Log, TEXT("Current Ammo : %d"), WeaponData.CurrentAmmo);
	//OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(CurrentAmmo, TotalAmmo);
}

void AWeapon::SetCurrentAndTotalAmmo(int32 _CurrentAmmo, int32 _TotalAmmo)
{
	WeaponData.CurrentAmmo = FMath::Max(0.f, _CurrentAmmo);
	WeaponData.TotalAmmo = FMath::Max(0.f, _TotalAmmo);
		
	UE_LOG(LogTemp, Log, TEXT("Current Ammo : %d"), WeaponData.CurrentAmmo);
	OnCurrentAmmoAndTotalAmmoChangeDelegate.Broadcast(WeaponData.CurrentAmmo, WeaponData.TotalAmmo);
}
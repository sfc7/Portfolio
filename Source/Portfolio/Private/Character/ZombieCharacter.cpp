// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ZombieCharacter.h"
#include "Game/FGameInstance.h"
#include "Engine/StreamableManager.h"
#include "ZombieCharacterSettings.h"
#include "Component/MonsterComponent.h"
#include "AI/ZombieAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AZombieCharacter::AZombieCharacter()
{
	MonsterComponent = CreateDefaultSubobject<UMonsterComponent>(TEXT("MonsterComponent"));

	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//const UZombieCharacterSettings* CDO = GetDefault<UZombieCharacterSettings>();
	//if (0 < CDO->ZombieCharacterMeshPaths.Num())
	//{
	//	for (FSoftObjectPath PlayerCharacterMeshPath : CDO->ZombieCharacterMeshPaths)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Path: %s"), *(PlayerCharacterMeshPath.ToString()));
	//	}
	//}
}

void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	//const UZombieCharacterSettings* CDO = GetDefault<UZombieCharacterSettings>();
	//int32 RandIndex = FMath::RandRange(0, CDO->ZombieCharacterMeshPaths.Num() - 1);
	//CurrentZombieCharacterMeshPath = CDO->ZombieCharacterMeshPaths[RandIndex];

	//// CharacterComponentÀÇ gameinstance ÀÌ¿ë
	//UFGameInstance* FGameInstance = Cast<UFGameInstance>(GetGameInstance());
	//if (IsValid(FGameInstance)) {
	//	AssetStreamableHandle = FGameInstance->StreamableManager.RequestAsyncLoad(
	//		CurrentZombieCharacterMeshPath,
	//		FStreamableDelegate::CreateUObject(this, &ThisClass::MeshAssetLoad)
	//	);
	//}
}


float AZombieCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	MonsterComponent->SetCurrentHp(MonsterComponent->GetCurrentHp() - ActualDamage);


	return ActualDamage;
}

void AZombieCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (MonsterComponent != nullptr)
	{
		MonsterComponent->SetMonsterName(FName("Zombie"));
	}
}

void AZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AZombieCharacter::MeshAssetLoad()
{
	AssetStreamableHandle->ReleaseHandle();
	TSoftObjectPtr<USkeletalMesh> LoadedAsset(CurrentZombieCharacterMeshPath);
	if (LoadedAsset.IsValid()) {
		//GetMesh()->SetSkeletalMesh(LoadedAsset.Get());
	}
}


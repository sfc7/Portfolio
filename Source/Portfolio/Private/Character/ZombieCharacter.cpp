// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ZombieCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Game/FGameInstance.h"
#include "Engine/StreamableManager.h"
#include "ZombieCharacterSettings.h"
#include "Component/CharacterComponent.h"
#include "Component/MonsterComponent.h"
#include "Components/CapsuleComponent.h"
#include "AI/ZombieAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/ZombieAnimInstance.h"
#include "Game/FPlayerState.h"
#include "Game/FGameState.h"
#include "Portfolio/Portfolio.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DamageEvents.h"

AZombieCharacter::AZombieCharacter()
{
	MonsterComponent = CreateDefaultSubobject<UMonsterComponent>(TEXT("MonsterComponent"));
	MonsterComponent->SetIsReplicated(true);

	AIControllerClass = AZombieAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	const UZombieCharacterSettings* CDO = GetDefault<UZombieCharacterSettings>();
	int32 RandIndex = FMath::RandRange(0, CDO->ZombieCharacterMeshPaths.Num() - 1);
	CurrentZombieCharacterMeshPath = CDO->ZombieCharacterMeshPaths[RandIndex];

	if (HasAuthority()) {
		UFGameInstance* FGameInstance = Cast<UFGameInstance>(GetGameInstance());
		if (IsValid(FGameInstance)) {
			AssetStreamableHandle = FGameInstance->StreamableManager.RequestAsyncLoad(
				CurrentZombieCharacterMeshPath,
				FStreamableDelegate::CreateUObject(this, &ThisClass::MeshAssetLoad)
			);
		}
	}
}

void AZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetMonsterComponent()->GetCurrentHp() < KINDA_SMALL_NUMBER) {
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName(TEXT("Hips")), 1.f);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName(*CurrentBoneName), 1.f);
	}
}

void AZombieCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ReplicateMesh)
}

float AZombieCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetMonsterComponent()->GetIsDead())
	{
		if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) {
			APlayerCharacter* CauserCharacter = Cast<APlayerCharacter>(DamageCauser);
			if (IsValid(CauserCharacter)) {
				UCharacterComponent* CharacterComponent = CauserCharacter->GetCharacterComponent();

				if (IsValid(CharacterComponent)) {
					CharacterComponent->SetCurrentEXP(CharacterComponent->GetCurrentEXP() + GetMonsterComponent()->GetMonsterExpValue());
					CharacterComponent->SetMoney(CharacterComponent->GetMoney() + 50.f);
				}
			}
		}

		return 0.f;
	}

	

	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	GetMonsterComponent()->SetCurrentHp(GetMonsterComponent()->GetCurrentHp() - ActualDamage);

	if (GetMonsterComponent()->GetCurrentHp() < KINDA_SMALL_NUMBER) {
		MonsterComponent->SetIsDead(true);
		IsDead_NetMulticast();
		
		APlayerCharacter* CauserCharacter = Cast<APlayerCharacter>(DamageCauser);
		if (IsValid(CauserCharacter)) {
			UCharacterComponent* CharacterComponent = CauserCharacter->GetCharacterComponent();

			if (IsValid(CharacterComponent)) {
				CharacterComponent->SetCurrentEXP(CharacterComponent->GetCurrentEXP() + GetMonsterComponent()->GetMonsterExpValue());
			}
		}
	}

	return ActualDamage;
}

void AZombieCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetMonsterComponent() != nullptr)
	{
		GetMonsterComponent()->SetMonsterName(FName("Zombie"));
	}

	ZombieAnimInstance = Cast<UZombieAnimInstance>(GetMesh()->GetAnimInstance());
	if (ZombieAnimInstance) {
		ZombieAnimInstance->AttackHit.AddUObject(this, &ThisClass::Attack_BasicHit);
		ZombieAnimInstance->AttackMontageEnd.AddUObject(this, &ThisClass::AttackMontageEnd);
		ZombieAnimInstance->DeathMontageEnd.AddUObject(this, &ThisClass::DestroyActor);
	}
}

void AZombieCharacter::ZombieHitted(APlayerCharacter* Player, FHitResult _HitResult)
{
	if (IsValid(Player)) {
		UCharacterComponent* CharacterComponent = Player->GetCharacterComponent();

		if (IsValid(CharacterComponent)) {
			CurrentBoneName = _HitResult.BoneName.ToString();

			if (uint8 HitPart = GetNumberFromHitPart(CurrentBoneName))
			{
				if (uint8 RecentGetMoney = GetMoneyFromHitPart(HitPart)) {
					CharacterComponent->SetMoney(CharacterComponent->GetMoney() + RecentGetMoney);
				}
			}
		}
	}
}

void AZombieCharacter::MeshAssetLoad()
{
	AssetStreamableHandle->ReleaseHandle();
	TSoftObjectPtr<USkeletalMesh> LoadedAsset(CurrentZombieCharacterMeshPath);
	if (LoadedAsset.IsValid()) {
		ReplicateMesh = LoadedAsset.Get();
		GetMesh()->SetSkeletalMesh(ReplicateMesh);
	}
}

void AZombieCharacter::Attack()
{
	if (IsValid(ZombieAnimInstance)) {
		int16 RandValue = FMath::RandRange(0, ZombieAnimInstance->AttackMontageArrayNum - 1); //AnimInstance는 Uobject이기 때문에 설정없이 relicate가 불가능하고 transient하기 때문에 값을 캐릭터에서 구해서 RPC로 구현

		if (MonsterComponent) {
			AZombieAIController* AIController = Cast<AZombieAIController>(GetController());

			if (IsValid(AIController)) {
				ZombieAnimInstance->PlayAttackMontage(RandValue);
				GetMonsterComponent()->SetIsAttacking(true);
				AIController->SetIsAttackingInBlackboard(true);
			}
		}

		Attack_NetMulticast(RandValue);
	}
}

void AZombieCharacter::Attack_NetMulticast_Implementation(int16 _MontageArrayNum)
{
	if (IsValid(ZombieAnimInstance)) {
		ZombieAnimInstance->PlayAttackMontage(_MontageArrayNum);
	}
}

void AZombieCharacter::Attack_BasicHit()
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;

	FVector RangeVector = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + RangeVector * 0.5f;
	FQuat ZRotation = FRotationMatrix::MakeFromZ(RangeVector).ToQuat();
	float HalfHeight = AttackRange * 0.5f + AttackRadius;

	bool Result = GetWorld()->SweepSingleByChannel(
		OUT HitResult,
		Center,
		Center + AttackRange * GetActorForwardVector(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	FColor DrawColor;

	if (Result && Cast<APlayerCharacter>(HitResult.GetActor())) {
		DrawColor = FColor::Green;
		APlayerCharacter* HitActor = Cast<APlayerCharacter>(HitResult.GetActor());
		UGameplayStatics::ApplyDamage(HitActor, AttackDamage, GetController(), this, NULL); 
	}
	else {
		DrawColor = FColor::Red;
	}
}

void AZombieCharacter::AttackMontageEnd()
{
	GetMonsterComponent()->SetIsAttacking(false);
	AZombieAIController* AIController = Cast<AZombieAIController>(GetController());
	if (IsValid(AIController)) {
		AIController->SetIsAttackingInBlackboard(false);
	}
}

void AZombieCharacter::DestroyActor()
{
	Destroy();
}

void AZombieCharacter::OnRep_Mesh()
{
	if (IsValid(ReplicateMesh)) {
		GetMesh()->SetSkeletalMesh(ReplicateMesh);
	}
}

uint8 AZombieCharacter::GetNumberFromHitPart(FString BoneName)
{
	if (BoneName.Contains(FString("Left")) || BoneName.Contains(FString("Right")))
	{
		return 1;
	}
	else if (BoneName.Contains(FString("Spine")) || BoneName.Contains(FString("Pelvis")))
	{
		return 2;
	}
	else if (BoneName.Contains(FString("Neck")) || BoneName.Contains(FString("Head")))
	{
		return 3;
	}
	else {
		return 4;
	}

	return 0;
}

uint8 AZombieCharacter::GetMoneyFromHitPart(uint8 HitPart)
{
	switch (HitPart) {
		case 1:
			return 20;
			break;
		case 2:
			return 30;
			break;
		case 3:
			return 100;
			break;
		case 4:
			return 20;
			break;
		default:
			break;
	}

	return 0;
}

void AZombieCharacter::IsDead_NetMulticast_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);


	AZombieAIController* AIController = Cast<AZombieAIController>(GetController());
	if (IsValid(AIController)) {
		AIController->EndAIController();
	}

	FTimerHandle DeathTimer;
	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ThisClass::DestroyActor, 10.0f, false);
}
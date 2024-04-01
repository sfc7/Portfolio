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
#include "Portfolio/Portfolio.h"

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

void AZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRagdoll) {
		CurrentRagDollPercent = FMath::FInterpTo(CurrentRagDollPercent, TargetRagDollPercent, DeltaTime, 10.f);

		FName PivotBoneName = FName(TEXT("spine1"));
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PivotBoneName, CurrentRagDollPercent);

		if (CurrentRagDollPercent - TargetRagDollPercent < KINDA_SMALL_NUMBER) {
			GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, false);
			bIsRagdoll = false;
		}

		if (GetMonsterComponent()->GetCurrentHp() < KINDA_SMALL_NUMBER) {
			GetMesh()->SetSimulatePhysics(true);
			GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName(TEXT("Hips")), 1.f);
			
			bIsRagdoll = false;
		}
	}
}

float AZombieCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	/*USER_LOG(LogUser, Log, TEXT("%f"), GetMonsterComponent()->GetCurrentHp());*/

	if (GetMonsterComponent()->GetIsDead())
	{
		return 0.f;
	}

	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	GetMonsterComponent()->SetCurrentHp(GetMonsterComponent()->GetCurrentHp() - ActualDamage);

	if (GetMonsterComponent()->GetCurrentHp() < KINDA_SMALL_NUMBER) {	
		MonsterComponent->SetIsDead(true);
		IsDead_NetMulticast();

		if (IsLocallyControlled()) {
			APlayerCharacter* CauserCharacter = Cast<APlayerCharacter>(DamageCauser);
			if (IsValid(CauserCharacter)) {
				AFPlayerState* CauserCharacterPlayerState = Cast<AFPlayerState>(CauserCharacter->GetPlayerState());

				if (IsValid(CauserCharacterPlayerState)) {
					CauserCharacterPlayerState->SetCurrentEXP(CauserCharacterPlayerState->GetCurrentEXP() + GetMonsterComponent()->GetMonsterExpValue());
				}
			}
		}
	}
	else {
		FName PivotBoneName = FName(TEXT("spine1"));
		GetMesh()->SetAllBodiesBelowSimulatePhysics(PivotBoneName, true);
		TargetRagDollPercent = 1.f;

		HittedRagdollRestoreTimerDelegate.BindUObject(this, &ThisClass::OnHittedRagdollRestoreTimerElapsed);
		GetWorld()->GetTimerManager().SetTimer(HittedRagdollRestoreTimer, HittedRagdollRestoreTimerDelegate, 1.f, false);
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



void AZombieCharacter::MeshAssetLoad()
{
	AssetStreamableHandle->ReleaseHandle();
	TSoftObjectPtr<USkeletalMesh> LoadedAsset(CurrentZombieCharacterMeshPath);
	if (LoadedAsset.IsValid()) {
		//GetMesh()->SetSkeletalMesh(LoadedAsset.Get());
	}
}

void AZombieCharacter::Attack()
{
	if (ZombieAnimInstance) {
		GetMonsterComponent()->SetIsAttacking(true);
		ZombieAnimInstance->PlayAttackMontage();
	}
}

void AZombieCharacter::Attack_BasicHit()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
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

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, ZRotation, DrawColor, false, 5.f);
}

void AZombieCharacter::AttackMontageEnd()
{
	GetMonsterComponent()->SetIsAttacking(false);
}

void AZombieCharacter::DestroyActor()
{
	Destroy();
}

void AZombieCharacter::OnHittedRagdollRestoreTimerElapsed()
{
	FName PivotBoneName = FName(TEXT("spine1"));
	TargetRagDollPercent = 0.f;
	CurrentRagDollPercent = 1.f;
	bIsRagdoll = true;

}

void AZombieCharacter::IsDead_NetMulticast_Implementation()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	//ZombieAnimInstance->PlayDeathMontage();

	AZombieAIController* AIController = Cast<AZombieAIController>(GetController());
	if (IsValid(AIController)) {
		AIController->EndAIController();
	}

	FTimerHandle DeathTimer;
	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ThisClass::DestroyActor, 10.0f, false);
}

	
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MonsterCharacter.h"
#include "ZombieCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AZombieCharacter : public AMonsterCharacter
{
	GENERATED_BODY()

	friend class UBTTask_Attack;
public:
	AZombieCharacter();

	void ZombieHitted(class APlayerCharacter* Player, FHitResult _HitResult);

	class UMonsterComponent* GetMonsterComponent() { return MonsterComponent; }

	UFUNCTION(BlueprintCallable)
	void NavLinkMantle(FVector _Destination, float _WallDistance, ANavLinkProxy* _NavLink);

	UFUNCTION(BlueprintCallable)
	void NavLinkFlail(FVector _Destination, float _Jumpheight);

	int32 GetRandomWalkBlendSpace() { return RandomWalkBlendSpace; }

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// MeshLoad
	UFUNCTION()
		void OnRep_Mesh();
	UFUNCTION()
		void MeshAssetLoad();
	//
	
	//Attack
	void Attack();

	UFUNCTION(NetMulticast, Reliable)
	void Attack_NetMulticast(int16 _MontageArrayNum);

	void Attack_BasicHit();

	void AttackMontageEnd();
	//

	//Hit
	void DestroyActor();

	UFUNCTION(NetMulticast, Reliable)
		void IsDead_NetMulticast();

	uint8 GetNumberFromHitPart(FString BoneName);

	uint8 GetMoneyFromHitPart(uint8 HitPart);
	//

	//Mantle
	UFUNCTION(NetMulticast, Reliable)
	void NavLinkMantle_NetMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void NavLinkFlail_NetMulticast();

	void NavLinkJumpMoveLocaiton();

	void NavLinkJumpEnd();
	//

public:
	class AZombieMantleNavLink* LastJumpNavLink;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
		TObjectPtr<class UMonsterComponent> MonsterComponent;
private:
	FSoftObjectPath	CurrentZombieCharacterMeshPath = FSoftObjectPath();

	TSharedPtr<struct FStreamableHandle> AssetStreamableHandle = nullptr;

	UPROPERTY()
		TObjectPtr<class UZombieAnimInstance> ZombieAnimInstance;

	UPROPERTY(ReplicatedUsing = OnRep_Mesh)
	USkeletalMesh* ReplicateMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))


	//ABP

	TArray<float> SpeedArray = { 50.f ,200.f, 200.f, 200.f, 200.f, 300.f };

	int32 RandomWalkBlendSpace;
	int32 RandomWalkBlendSpaceCount = 6;

	//Attack
	float AttackDamage = 10.f;
	
	float AttackRange = 50.f;

	float AttackRadius = 50.f;
	//


	//Ragdoll
	
	FString CurrentBoneName;
	//

	//Mantle
	FVector NavLinkDestination;
};

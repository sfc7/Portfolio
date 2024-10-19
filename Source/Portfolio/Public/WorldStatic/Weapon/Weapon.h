// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Weapon.generated.h"

UCLASS()
class PORTFOLIO_API AWeapon : public AActor, public IInteractionInterface 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	
	virtual void Tick(float DeltaTime) override;

	int32 GetTotalAmmo() { return TotalAmmo; }

	int32 GetReloadMaxAmmo() { return ReloadMaxAmmo; }
	
	virtual void BeginFocus();
	virtual void EndFoucs();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact();

	UFUNCTION(Server, Reliable)
		void SpawnMuzzleFlash_Server();

	UFUNCTION(NetMulticast, Reliable)
		void SpawnMuzzleFlash_NetMulticast();

	UFUNCTION(Server, Reliable)
		void SetWeaponMesh_Server(USkeletalMesh* _Mesh);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
		void OnRep_WeaponMesh();

public:
	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairCenter;

	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairLeft;

	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairRight;

	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairTop;

	UPROPERTY(EditAnywhere, Category = CrossHair)
		class UTexture2D* CrossHairBottom;

	UPROPERTY(EditAnywhere)
		UParticleSystem* ImpactEffect;
protected:
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponMesh)
		USkeletalMesh* ReplicateMesh;

	UPROPERTY(EditAnywhere)
		int32 TotalAmmo;

	UPROPERTY(EditAnywhere)
		int32 ReloadMaxAmmo;

	UPROPERTY(EditAnywhere)
		FString WeaponName;
	
	FInteractableData InstanceInteractableData;

	UPROPERTY(EditAnywhere)
		UParticleSystem* MuzzleFlash;

	



};

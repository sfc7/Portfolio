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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



protected:
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
		int32 TotalAmmo;

	UPROPERTY(EditAnywhere)
		int32 ReloadMaxAmmo;

	UPROPERTY(EditAnywhere)
		FString WeaponName;
	
	FInteractableData InstanceInteractableData;
};

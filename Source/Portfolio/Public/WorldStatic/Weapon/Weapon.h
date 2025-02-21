// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Data/DataStruct.h"
#include "Weapon.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentAmmoAndTotalAmmoChangeDelegate, int32, CurrentAmmo, int32, TotalAmmo);


UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Initial UMETA(DisplayName = "Initial State"),
	Equipped UMETA(DisplayName = "Equipped"),
	UnEquipped UMETA(DisplayName = "UnEquipped")
};


UCLASS()
class PORTFOLIO_API AWeapon : public AActor, public IInteractionInterface 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	
	virtual void Tick(float DeltaTime) override;


	FWeaponData& GetWeaponData() { return WeaponData; }

	int32 GetCurrentAmmo() { return WeaponData.CurrentAmmo; }

	int32 GetTotalAmmo() { return WeaponData.TotalAmmo; }

	int32 GetReloadMaxAmmo() { return WeaponData.ReloadMaxAmmo; }
	
	void SetReloadMaxAmmo(int32 _ReloadMaxAmmo);

	void SetTotalAmmo(int32 _TotalAmmo); 

	void SetCurrentAmmo(int32 _CurrentAmmo);

	void SetCurrentAndTotalAmmo(int32 _CurrentAmmo, int32 _TotalAmmo);

	UFUNCTION(Server, Reliable)
		void SetWeaponData_Server(FWeaponData _WeaponData);
		

	void SetWeaponData(FWeaponData _WeaponData);

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

		void SetWeaponMesh(USkeletalMesh* _Mesh);

		void SetWeaponState(EWeaponState _State);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
		void OnRep_WeaponMesh();

	UFUNCTION()
		void OnRep_WeaponState();
public:
	FOnCurrentAmmoAndTotalAmmoChangeDelegate OnCurrentAmmoAndTotalAmmoChangeDelegate;

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


	UPROPERTY(ReplicatedUsing = OnRep_WeaponMesh)
		USkeletalMesh* ReplicateMesh;

	UPROPERTY(Replicated, EditAnywhere)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
		USceneComponent* RootScene;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpotLightComponent* MuzzleSpotLightComponent;

	UPROPERTY(EditAnywhere)
		class UMaterial* MuzzleLightMaterial;

	FSoftObjectPath	MuzzleLightMaterialPath = FSoftObjectPath();

	TSharedPtr<struct FStreamableHandle> AssetStreamableHandle = nullptr;

protected:
	UPROPERTY(EditAnywhere)
		FString WeaponName;

	UPROPERTY(Replicated, EditAnywhere)
		FWeaponData WeaponData;

	FInteractableData InstanceInteractableData;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, EditAnywhere)
		EWeaponState WeaponState;

};

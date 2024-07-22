// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"



UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	None UMETA(DisplayName = "None"),
	Equip UMETA(DisplayName = "Equip"),
	Buy UMETA(DisplayName = "Drop"),
	End UMETA(DisplayName = "End")
};

UCLASS()
class PORTFOLIO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	
	virtual void Tick(float DeltaTime) override;

	void ShowPickUpText(bool ShowFlag);

	void SetWeaponState(EWeaponState _WeaponState);

	class USphereComponent* GetSphereComponent() const { return SphereComponent; }

	uint32 GetTotalAmmo() { return TotalAmmo; }

	uint32 GetReloadMaxAmmo() { return ReloadMaxAmmo; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION()
		void OnRep_WeaponState();

protected:
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
		USphereComponent* SphereComponent;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere)
		EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* PickUpText;

	UPROPERTY(VisibleAnywhere)
		uint32 TotalAmmo;

	UPROPERTY(VisibleAnywhere)
		uint32 ReloadMaxAmmo;







};

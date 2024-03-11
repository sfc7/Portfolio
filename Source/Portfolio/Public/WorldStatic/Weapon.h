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
	Drop UMETA(DisplayName = "Drop"),
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

	FORCEINLINE void SetWeaponState(EWeaponState _WeaponState) { WeaponState = _WeaponState; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
		class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
		EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* PickUpText;




};

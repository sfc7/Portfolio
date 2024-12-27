// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

UCLASS()
class PORTFOLIO_API AGrenade : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
		TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere)
		TObjectPtr<class UParticleSystem> ImpactParticle;

	UPROPERTY()
		TObjectPtr<class URadialForceComponent> RadialForceComponent;

	UPROPERTY()
		TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
		TObjectPtr<class UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(ReplicatedUsing = OnRep_Velocity)
		FVector GrenadeVelocity;

	UPROPERTY(EditAnywhere)
		float GrenadeInRadius = 200.f;

	UPROPERTY(EditAnywhere)
		float GrenadeOutRadius = 500.f;
private:
	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
		float DestroyTime = 2.5f;
public:	
	AGrenade();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	UFUNCTION()
		void OnRep_Velocity();

	UFUNCTION(Server, UnReliable)
		void SetVelocity_Server(const FVector& _Velocity);

	void SetVelocity(const FVector& _Velocity);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UProjectileMovementComponent* GetProjectileMovementComponent() { return ProjectileMovementComponent; }
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnBounce(const FHitResult& _ImpactResult, const FVector& _ImpactVelocity);

	UFUNCTION(NetMulticast, Reliable)
		void FireImpulse_NetMulticast();

	void StartDestroyTimer();
	void DestroyTimerFinish();
	void ApplyDamage();



};

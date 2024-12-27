// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldStatic/Weapon/Grenade.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Controller/PlayerCharacterController.h"
#include "Character/PlayerCharacter.h"
#include "Character/ZombieCharacter.h"
#include "Portfolio/Portfolio.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h" 

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.1f;
	ProjectileMovementComponent->BounceVelocityStopSimulatingThreshold = 400.0f; 

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->Radius = GrenadeOutRadius;
	RadialForceComponent->ForceStrength = 150.f;
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->AddCollisionChannelToAffect(ECC_GameTraceChannel7);

	RadialForceComponent->SetIsReplicated(true);
}

void AGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, GrenadeVelocity);
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();

	StartDestroyTimer();
}

void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrenade::Destroyed()
{
	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	}

	ApplyDamage();

	Super::Destroyed();
}

void AGrenade::OnBounce(const FHitResult& _ImpactResult, const FVector& _ImpactVelocity)
{
}

void AGrenade::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AGrenade::DestroyTimerFinish,
		DestroyTime
	);

	FireImpulse_NetMulticast();
}

void AGrenade::DestroyTimerFinish()
{
	Destroy();
}

void AGrenade::ApplyDamage()
{
	FireImpulse_NetMulticast();

	AActor* DamageInstigator = GetInstigator();
	if (DamageInstigator && HasAuthority())
	{
		APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>(DamageInstigator->GetInstigatorController());
		if (PlayerCharacterController)
		{
			TArray<AActor*> IgnoreActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), IgnoreActors);

			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				150.f,
				50.f, 
				GetActorLocation(),
				GrenadeInRadius,
				GrenadeOutRadius,
				1.f,
				UDamageType::StaticClass(),
				IgnoreActors,
				DamageInstigator,
				PlayerCharacterController,
				ECC_GameTraceChannel7
			);	
		}

		RadialForceComponent->FireImpulse();
	}
}

void AGrenade::SetVelocity(const FVector& _Velocity)
{
	GrenadeVelocity = _Velocity;
	ProjectileMovementComponent->Velocity = _Velocity;
}

void AGrenade::SetVelocity_Server_Implementation(const FVector& _Velocity)
{
	GrenadeVelocity = _Velocity;
	ProjectileMovementComponent->Velocity = _Velocity;
}

void AGrenade::FireImpulse_NetMulticast_Implementation()
{
	if (!HasAuthority()) {
		RadialForceComponent->FireImpulse();
	}
}

void AGrenade::OnRep_Velocity()
{
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Velocity = GrenadeVelocity;
	}
}